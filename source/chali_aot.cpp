/**
 * @file chali_aot.cpp
 * @brief 将优化后的 ChaLi 四元式直接降低为 LLVM 中间表示并生成原生产物。
 */
#include "chali_aot.hpp"
#include "ast/ast.hpp"
#include "chali_aot_debug.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_aot_runtime_assets.hpp"
#include "chali_stdlib.hpp"
#include "glue/std_aot_modules.hpp"
#include "chali_temp.hpp"
#include "console/utf8.hpp"
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
namespace chali_aot
{
namespace
{
/**
 * @brief 将四元式符号中的标点编码为 LLVM 标识符可用的字符。
 * @param name 原始符号名。
 * @return 不含标点的标识符片段。
 */
Memory::String sanitize(const Memory::String &name)
{
    Memory::String out;
    for (const char c : name)
    {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        out += ok ? c : '_';
    }
    return out;
}

/**
 * @brief 将整数偏移或计数转换为十进制文本。
 * @param value 待表示的有符号整数。
 * @return 可写入 LLVM 模块的十进制文本。
 */
Memory::String decimal(long long value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%lld", value);
    return Memory::String(buffer);
}

/**
 * @brief 识别包含小数点或指数部分的浮点立即数。
 * @param token 四元式操作数文本。
 * @return 具有浮点词法形态时返回 true。
 */
bool literalIsFloat(const Memory::String &token)
{
    if (token.empty()) return false;
    bool digit = false;
    for (std::size_t index = 0; index < token.size(); ++index)
    {
        const char c = token[index];
        if (c == '.') return true;
        if ((c == 'e' || c == 'E') && digit && index + 1u < token.size()) return true;
        if (c >= '0' && c <= '9') digit = true;
    }
    return false;
}

/**
 * @brief 判断操作数字段是否表示类型或存储类别。
 * @param token 待检查的四元式字段。
 * @return 属于已知类型或存储类别时返回 true。
 */
bool isTypeName(const Memory::String &token)
{
    static const std::set<std::string> names = {
        "i8",
        "i16",
        "i32",
        "i64",
        "u8",
        "u16",
        "u32",
        "u64",
        "f32",
        "f64",
        "bool",
        "char",
        "string",
        "void",
        "local",
        "global",
        "param",
        "field",
        "const",
    };
    return names.count(std::string(token.c_str())) != 0u;
}

/**
 * @brief 区分四元式符号与数字立即数。
 * @param token 待检查的操作数。
 * @return 非空且不以数字或正负号开头时返回 true。
 */
bool literalIsName(const Memory::String &token)
{
    if (token.empty()) return false;
    const char first = token.front();
    return !(first == '-' || first == '+' || (first >= '0' && first <= '9'));
}

/* ------------------------------------------------------------ 类型与形态 */

/**
 * @brief 一个值的种类（决定 LLVM 类型与输出方法）。
 */
enum class Kind
{
    Int,     ///< 整数与地址（统一使用 i64）。
    Float,   ///< `double`。
    String,  ///< 字符串：值仍是地址，但打印与释放按串处理。
};

/** @brief 类型后缀 → 位宽与有无符号、是否浮点。 */
struct Width
{
    int bits = 64;
    bool unsigned_ = false;
    bool float_ = false;
};

/**
 * @brief 拆分四元式操作符及其类型后缀，保留赋值操作符。
 * @param op 完整操作符。
 * @param base 接收不含类型后缀的操作符。
 * @param suffix 接收类型后缀；没有后缀时清空。
 */
void splitOperation(const Memory::String &op, Memory::String &base, Memory::String &suffix)
{
    /* `:=` 不是"带类型后缀的运算"——按 find(':') 切会切出 base="" / suffix="="，
     * 于是赋值目标被当成整数重新标一遍，`float64 fromInt = 2` 这类就丢了浮点种类。 */
    const std::size_t colon = op.find(':');
    if (colon == Memory::String::npos || colon == 0u)
    {
        base = op;
        suffix.clear();
        return;
    }
    base = op.substr(0, colon);
    suffix = op.substr(colon + 1u);
}

/**
 * @brief 识别结果为布尔整数的比较和逻辑操作。
 * @param base 不含类型后缀的操作符。
 * @return 属于比较或逻辑操作时返回 true。
 */
bool isLogicalOperator(const Memory::String &base)
{
    return base == "eq" || base == "ne" || base == "lt" || base == "le" || base == "gt" || base == "ge" || base == "==" || base == "!=" || base == "<" || base == "<=" || base == ">" || base == ">=" || base == "and" || base == "or" || base == "not";
}

/**
 * @brief 解析四元式类型后缀的位宽、符号和浮点类别。
 * @param suffix i8、u32、f64 等后缀；空值按 i64。
 * @return 解析后的标量位宽信息。
 */
Width widthOf(const Memory::String &suffix)
{
    Width width;
    if (suffix.empty()) return width;
    if (suffix == "f32")
    {
        width.bits = 32;
        width.float_ = true;
        return width;
    }
    if (suffix == "f64")
    {
        width.bits = 64;
        width.float_ = true;
        return width;
    }
    if (!suffix.empty() && suffix.front() == 'u') width.unsigned_ = true;
    Memory::String digits = suffix;
    if (!digits.empty() && (digits.front() == 'u' || digits.front() == 'i')) digits = digits.substr(1u);
    int value = 64;
    if (!digits.empty())
    {
        value = 0;
        for (const char c : digits)
            if (c >= '0' && c <= '9') value = value * 10 + (c - '0');
    }
    width.bits = value == 0 ? 64 : value;
    return width;
}

/**
 * @brief 从声明的四元式类型推断统一存储类别。
 * @param name 声明使用的四元式类型名。
 * @return f32 和 f64 返回浮点类别，其余返回整数或句柄类别。
 */
Kind declarationKind(const Memory::String &name)
{
    return name == "f32" || name == "f64" ? Kind::Float : Kind::Int;
}

/* ------------------------------------------------------------ 生成器主体 */

/**
 * @brief 一个 ChaLi 函数（或一段清理块）在 LLVM 模块中的函数范围。
 */
struct Function
{
    Memory::String label;                       ///< 四元式标签。
    Memory::String llvmName;                    ///< 生成的 LLVM 函数名。
    std::size_t begin = 0;                      ///< 起始四元式下标（含标签之前的形参 decl）。
    std::size_t labelIndex = 0;                 ///< 入口 `label` 这条四元式的下标。
    std::size_t end = 0;                        ///< 结束下标（不含）。
    Memory::Vector<Memory::String> parameters;  ///< 形参名（按序）。
    bool exported = false;                      ///< 是否导出给 C。
    bool returnsFloat = false;                  ///< `ret` 出来的是不是浮点。
    bool returnsString = false;                 ///< `ret` 出来的是不是字符串。
};

/**
 * @brief 一次 AOT 翻译的全部中间状态。
 */
struct Translator
{
    const Request &request;
    Memory::Vector<Memory::String> *diagnostics;
    TargetInfo target; ///< 产物目标，与编译器运行宿主分开。

    std::vector<Function> functions;
    std::unordered_map<std::string, std::size_t> functionByLabel;  ///< 标签 → functions 下标。
    std::unordered_map<std::string, std::string> variableName;     ///< 四元式名字 → LLVM 符号名。
    std::unordered_map<std::string, Kind> kinds;                   ///< 名字 → 种类。
    std::unordered_map<std::string, std::string> globals;          ///< 全局量 → LLVM 符号名（`g_x`）。
    std::unordered_map<std::string, std::size_t> dataOffsets;      ///< 数据区符号 → 偏移。
    std::unordered_map<std::string, const ExternFunction *> externsByLabel;
    std::set<std::string> usedExternLabels;  ///< 仅为可达调用生成外部声明。
    /* 从 `main`（与 `--export` 点名的函数）出发沿 `call` 边能走到的标签。
     * 标准库里绝大多数段落这份程序根本不会跑，走不到的就不翻译——
     * 仅翻译实际可达函数，链接完整、语义不变，翻译量却降一个数量级。 */
    std::set<std::string> reachable;
    std::map<std::string, std::size_t> nativeNames;               ///< 本模块实际调用的原生服务名称。
    std::map<std::string, std::size_t> sourceFiles;               ///< 可达指令的源码文件，用于原生错误定位。
    std::set<std::string> addressedFunctions;                     ///< 数据映像中实际保存了地址的函数。
    std::set<std::string> callbackReachable;                      ///< 可从函数值回调到达的语言函数。
    Memory::Vector<stackvm::LayoutRow> dataRows;                 ///< 解码后的数据区布局，供格式串直连取字面量字节。
    bool dataRowsReady = false;                                   ///< dataRows 是否已解码。
    std::string formatScratch;                                    ///< formatBytesOf 的单槽缓存。
    std::vector<std::string> printLiterals;                       ///< `__std_print` 直连发射的字面量段常量。
    bool watchdogUsed = false;                                    ///< 可达程序使用看门狗时生成逻辑指令安全点。
    bool profileOwnership = false;                                ///< 显式启用时插入所有权操作计数，不用于正式计时。
    bool cacheAddresses = true;                                   ///< 只在无失效屏障的直线路径复用已验证地址。
    bool bulkCopies = true;                                       ///< 聚合槽复制采用保留重叠和陷阱回退的原生助手。
    bool inlineHints = true;                                      ///< 为适用的极小内部 AOT 函数提供原生内联提示。
    bool directFunctionValues = true;                             ///< 只将直线路径内已知的无环境函数值改为直接调用。
    bool rawIntrinsics = true;                                    ///< 将原始存储调用直接连接标量内核，内联小型指针包装。
    std::map<std::size_t, Memory::String> functionAddresses;      ///< 不可变函数标签数据地址到原入口。
    std::map<std::string, std::size_t> profileSites;              ///< 操作种类、函数和四元式位置对应的常量字符串。
    std::map<std::string, std::set<std::string>> temporaryUsers;  ///< 临时符号的实际使用函数，用于识别跨函数共享值。
    NativeDebugInfo debugInfo; ///< 原生源码位置与变量元数据。
    std::map<std::string, const ast::Function *> astFunctions; ///< 直接后端的函数 AST。
    std::set<std::string> astInlineFunctions; ///< 已证明无递归且符合展开预算的 AST 函数。
    std::map<std::string, Memory::Map<Memory::String, Memory::String>> astSymbols; ///< 函数内名字的独立存储身份。
    std::map<const ast::Node *, std::size_t> astData; ///< AST 常量节点的数据地址。
    Memory::Map<Memory::String, ast::TypePtr> astTypes; ///< AST 类型查询环境。

    /**
     * @brief 保存本次四元式分析请求和诊断输出。
     * @param in 本次 AOT 请求。
     * @param out 可选的诊断容器。
     */
    explicit Translator(const Request &in, Memory::Vector<Memory::String> *out) : request(in), diagnostics(out)
    {
        Memory::String targetError;
        if (!describeTarget(request.options.target, target, &targetError)) fail(targetError);
        if (!request.options.allocator.empty() && request.options.allocator != "malloc" && request.options.allocator != "pool") fail("AOT：--allocator 只接受 malloc 或 pool");
        watchdogUsed = request.options.stepLimit != 0u;
        const auto *flag = std::getenv("CHALI_BACKEND_PROFILE");
        profileOwnership = flag && *flag && std::strcmp(flag, "0") != 0;
        const auto *cache = std::getenv("CHALI_NO_AOT_ADDRESS_CACHE");
        cacheAddresses = !(cache && *cache);
        const auto *bulk = std::getenv("CHALI_NO_AOT_BULK_COPY");
        bulkCopies = !(bulk && *bulk);
        const auto *hint = std::getenv("CHALI_NO_AOT_INLINE_HINT");
        inlineHints = !(hint && *hint);
        if (request.options.debug) inlineHints = false;
        const auto *direct = std::getenv("CHALI_NO_AOT_DIRECT_CALLS");
        directFunctionValues = !(direct && *direct);
        const auto *raw = std::getenv("CHALI_NO_AOT_RAW_INTRINSICS");
        rawIntrinsics = !(raw && *raw);
    }

    /**
     * @brief 登记翻译错误并限制级联诊断数量。
     * @param message 实际失败原因。
     */
    void fail(const Memory::String &message)
    {
        if (diagnostics != nullptr && diagnostics->size() < 32u) diagnostics->push_back(message);
    }

    /**
     * @brief 取数据区字面量符号的原始字节。
     * @param token 字面量的数据区符号名。
     * @return 字面量内容字节；布局不可用或符号不在数据区时返回空指针。单槽缓存，连续查询不同符号会覆盖。
     */
    const std::string *formatBytesOf(const Memory::String &token)
    {
        if (!dataRowsReady)
        {
            dataRowsReady = true;
            decodeDataLayout(request.dataLayout, dataRows);
        }
        for (const auto &row : dataRows)
        {
            if (row.name != token || !row.hasSize) continue;
            if (row.offset > request.dataBytes.size() || row.size > request.dataBytes.size() - row.offset) continue;
            formatScratch.assign(request.dataBytes.data() + row.offset, row.size);
            const auto terminator = formatScratch.find('\0');
            if (terminator == std::string::npos) return nullptr;
            formatScratch.resize(terminator);
            return &formatScratch;
        }
        return nullptr;
    }

    /**
     * @brief 为四元式符号分配可重复查询的唯一模块名称。
     * @param name 原始符号。
     * @param prefix 区分名称用途的前缀。
     * @return 已登记或新分配的符号名称。
     */
    Memory::String identifier(const Memory::String &name, const Memory::String &prefix)
    {
        auto found = variableName.find(std::string(name.c_str()));
        if (found != variableName.end()) return Memory::String(found->second.c_str());
        const Memory::String made = prefix + decimal(static_cast<long long>(variableName.size())) + "_" + sanitize(name);
        variableName.emplace(std::string(name.c_str()), std::string(made.c_str()));
        return made;
    }

    /**
     * @brief 解析声明的全局符号或分配普通符号名称。
     * @param name 四元式变量名。
     * @return 生成 LLVM 全局槽和局部槽所用的名称。
     */
    Memory::String use(const Memory::String &name)
    {
        /* 全局量（decl 的第二个格子写 `global`）是全程序一份，不能当局部形参见谁谁传。 */
        const auto found = globals.find(std::string(name.c_str()));
        if (found != globals.end()) return Memory::String(found->second.c_str());
        return identifier(name, "v");
    }

    /**
     * @brief 读取符号的值类别。
     * @param name 待查询符号。
     * @return 已推断类别；未知符号按整数处理。
     */
    Kind kindOf(const Memory::String &name)
    {
        const auto found = kinds.find(std::string(name.c_str()));
        return found == kinds.end() ? Kind::Int : found->second;
    }

    /**
     * @brief 更新非立即数符号的类别并保留字符串标记。
     * @param name 目标符号。
     * @param kind 拟登记的值类别。
     * @return 实际类别发生变化时返回 true。
     */
    bool markKind(const Memory::String &name, Kind kind)
    {
        if (name.empty() || !literalIsName(name)) return false;
        const auto found = kinds.find(std::string(name.c_str()));
        if (found != kinds.end() && (found->second == Kind::String || found->second == kind)) return false; /* 一旦是串就不再改 */
        kinds[std::string(name.c_str())] = kind;
        return true;
    }

    /**
     * @brief 标记一个有效符号为字符串句柄。
     * @param name 承载字符串句柄的四元式符号。
     */
    void markString(const Memory::String &name)
    {
        if (name.empty() || !literalIsName(name)) return;
        kinds[std::string(name.c_str())] = Kind::String;
    }

    /**
     * @brief 查找数据符号在初始化映像中的字节偏移。
     * @param symbol 数据区符号名。
     * @return 找到时返回非负偏移，否则返回 -1。
     */
    std::int64_t offsetOf(const Memory::String &symbol) const
    {
        const auto found = dataOffsets.find(std::string(symbol.c_str()));
        return found == dataOffsets.end() ? -1 : static_cast<std::int64_t>(found->second);
    }
};

/**
 * @brief 解码数据布局并建立数据符号偏移表。
 * @param state 接收布局映射及解析诊断的分析状态。
 */
void loadLayout(Translator &state)
{
    Memory::Vector<stackvm::LayoutRow> rows;
    if (!stackvm::decodeDataLayout(state.request.dataLayout, rows))
    {
        state.fail("AOT：读不懂数据区布局");
        return;
    }
    for (const auto &row : rows) state.dataOffsets.emplace(std::string(row.name.c_str()), row.offset);
}

/**
 * @brief 从函数入口、声明和调用关系推断签名、类型及可达函数。
 * @param state 持有输入四元式并接收分析结果的状态。
 */
void analyze(Translator &state)
{
    const stackvm::QuadFile *file = state.request.quads;
    if (file == nullptr)
    {
        state.fail("AOT：没有四元式输入");
        return;
    }
    const Memory::Vector<stackvm::Quad> &quads = file->quads;
    for (const auto &function : state.request.externs)
    {
        state.externsByLabel.emplace(std::string(function.label.c_str()), &function);
    }

    /* 1) 哪些标签是函数入口：被 `call` 点名的，加上 `main`。
     * 同时把所有 `label` 的名字收下来——跳转标签不是变量，别混进形参表。 */
    std::set<std::string> entryLabels{ "main", "__chali_library_init", "__chali_library_finish" };
    std::set<std::string> labelNames;
    for (const auto &quad : quads)
    {
        if (quad.op == "call" && !quad.args.empty()) entryLabels.insert(std::string(quad.arg(0).c_str()));
        if (quad.op == "label" && quad.args.size() >= 3u) labelNames.insert(std::string(quad.arg(2).c_str()));
        if (quad.op == "label" && !quad.arg(2).empty())
        {
            const auto label = quad.arg(2);
            const auto address = state.request.dataBytes.find(label + Memory::String(1, '\0'));
            if (address != Memory::String::npos && (address == 0 || state.request.dataBytes[address - 1] == '\0'))
            {
                entryLabels.insert(std::string(label.c_str()));
                state.addressedFunctions.insert(std::string(label.c_str()));
                state.functionAddresses[address] = label;
            }
        }
    }
    // 组装器的形参候选表可能把局部声明后的内部标签当成入口。
    // 无回边的块表达式也有循环标签，不能只靠跳转目标过滤；内部辅助函数以真实调用或函数值为准。
    std::set<std::string> branchTargets;
    for (const auto &quad : quads)
        if (quad.op == "j" || quad.op == "jz" || quad.op == "jnz") branchTargets.insert(std::string(quad.arg(2).c_str()));
    if (state.request.program != nullptr)
        for (const auto &entry : state.request.program->parameters)
            if (branchTargets.count(std::string(entry.first.c_str())) == 0u && entry.first.rfind("__chali_", 0u) != 0u)
                entryLabels.insert(std::string(entry.first.c_str()));
    for (const auto &entry : state.request.options.exports) entryLabels.insert(std::string(entry.c_str()));

    /* 2) 切段：每个函数入口到下一个入口之间是一段。
     * ChaLi 把形参的 `decl` 排在函数标签**之前**（紧跟着上一段的 `ret`），所以新段的起点
     * 要往前吞掉这一串 `decl`；上一段的终点同步回退，别把别人的形参声明抢走。 */
    std::size_t index = 0;
    while (index < quads.size())
    {
        const auto &quad = quads[index];
        if (quad.op != "label" || quad.args.size() < 3u || entryLabels.count(std::string(quad.arg(2).c_str())) == 0u)
        {
            ++index;
            continue;
        }
        std::size_t begin = index;
        while (begin > 0u && quads[begin - 1u].op == "decl") --begin;
        Function function;
        function.label = quad.arg(2);
        function.llvmName = "f_" + sanitize(function.label);
        if (sanitize(function.label) != function.label)
            function.llvmName = "f_" + decimal(static_cast<long long>(state.functions.size())) + "_" + sanitize(function.label);
        function.begin = begin;
        function.labelIndex = index;
        if (!state.functions.empty()) state.functions.back().end = begin; /* 交出尾巴上的形参 decl */
        std::size_t end = index + 1u;
        while (end < quads.size())
        {
            const auto &next = quads[end];
            if (next.op == "label" && next.args.size() >= 3u && entryLabels.count(std::string(next.arg(2).c_str())) != 0u) break;
            if (next.op == "halt") break;
            ++end;
        }
        function.end = end;
        if (state.request.program != nullptr)
        {
            const auto found = state.request.program->parameters.find(function.label);
            if (found != state.request.program->parameters.end()) function.parameters = found->second;
        }
        state.functionByLabel.emplace(std::string(function.label.c_str()), state.functions.size());
        state.functions.push_back(function);
        index = end;
    }
    /* 2.1 全局量：`decl(<类型>, global, 名字)`。它们全程序一份，
     * 既不进形参表也不在函数里声明局部——`const int64 N = 4000000` 就是这一类。 */
    for (const auto &quad : quads)
    {
        if (quad.op != "decl" || quad.args.size() < 3u) continue;
        if (quad.arg(1) != "global") continue;
        const Memory::String &name = quad.arg(2);
        if (name.empty() || name == "_") continue;
        state.globals.emplace(std::string(name.c_str()), "g_" + decimal(static_cast<long long>(state.globals.size())) + "_" + sanitize(name));
    }
    /* 2.2 形参表：入口标签之前那串 `decl` 就是形参声明，按出现顺序。
     * 这比"只读不写"的推断准得多——`String.length` 的 `self`、`half` 的 `x$193` 都靠它认出来。 */
    for (auto &function : state.functions)
        for (std::size_t at = function.begin; at < function.labelIndex; ++at)
        {
            const auto &quad = quads[at];
            if (quad.op != "decl" || quad.args.size() < 3u) continue;
            const Memory::String &name = quad.arg(2);
            if (name.empty() || name == "_") continue;
            if (std::find(function.parameters.begin(), function.parameters.end(), name) != function.parameters.end()) continue;
            function.parameters.push_back(name);
        }
    /* 2.5 形参推断：段内只被读、从没被写过的名字就是形参。
     * `program` 给不出参数表时（AOT 直接读 `.quad` 就是这种情况）靠这一遍补齐——
     * 少了它，`__chali_len` 这类形参在产物里就是没声明的标识符。 */
    for (auto &function : state.functions)
    {
        std::set<std::string> written;
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = quads[at];
            if (quad.args.size() < 3u) continue;
            const Memory::String &target = quad.arg(2);
            if (!literalIsName(target)) continue;
            if (quad.op == "decl" || quad.op == ":=" || quad.op == "call" || quad.op == "addr" || quad.op == "alloc" || quad.op.rfind("load", 0) == 0u || quad.op.find(':') != Memory::String::npos)
                written.insert(std::string(target.c_str()));
        }
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = quads[at];
            /* `decl(i64, local, v)` 的前两格是类型与存储类，`call(f, ...)` 的第一格是被调者，
             * 跳转指令的第一格之外还有标签——它们都不是变量，别混进形参表里。 */
            if (quad.op == "decl" || quad.op == "label") continue;
            for (std::size_t slot = 0u; slot + 1u < quad.args.size() && slot < 2u; ++slot)
            {
                if (slot == 0u && quad.op == "call") continue;
                const Memory::String &name = quad.args[slot];
                if (name.empty() || name == "_" || !literalIsName(name)) continue;
                if (isTypeName(name)) continue;                                         /* 类型名不是变量 */
                if (name.rfind("__temp_", 0) == 0u) continue;                           /* 临时量必然会被写 */
                if (state.dataOffsets.count(std::string(name.c_str())) != 0u) continue; /* 数据区符号不是变量 */
                if (state.globals.count(std::string(name.c_str())) != 0u) continue;     /* 全局量不是形参 */
                if (entryLabels.count(std::string(name.c_str())) != 0u) continue;       /* 函数标签 */
                if (labelNames.count(std::string(name.c_str())) != 0u) continue;        /* 跳转标签 */
                if (written.count(std::string(name.c_str())) != 0u) continue;
                if (std::find(function.parameters.begin(), function.parameters.end(), name) != function.parameters.end()) continue;
                function.parameters.push_back(name);
            }
        }
    }
    /* 函数入口可能重复出现（同名标签只留第一段）。 */
    for (const auto &name : state.request.options.exports)
    {
        const auto found = state.functionByLabel.find(std::string(name.c_str()));
        if (found != state.functionByLabel.end())
        {
            auto &function = state.functions[found->second];
            function.exported = true;
            if (std::any_of(state.request.abiExports.begin(), state.request.abiExports.end(), [&](const auto &entry) { return entry.label == function.label; }))
                function.llvmName = "__chali_impl_" + sanitize(function.label);
            else if (function.label.rfind("program::", 0) == 0)
                function.llvmName = "f_" + sanitize(function.label.substr(9));
        }
    }
    if (state.functions.empty()) state.fail("AOT：四元式里没有可翻译的函数");

    /* 4) 种类推断：先按 decl 与运算目标打底，再沿 `:=` 传播浮点 / 字符串。 */
    for (std::size_t at = 0; at < quads.size(); ++at)
    {
        const auto &quad = quads[at];
        if (quad.op == "decl" && quad.args.size() >= 3u)
        {
            state.markKind(quad.arg(2), declarationKind(quad.arg(0)));
        }
        Memory::String base, suffix;
        splitOperation(quad.op, base, suffix);
        if (base == "cvt")
        {
            const Width width = widthOf(suffix);
            if (quad.args.size() >= 3u) state.markKind(quad.arg(2), width.float_ ? Kind::Float : Kind::Int);
            continue;
        }
        if (!suffix.empty() && quad.args.size() >= 3u)
        {
            const Width width = widthOf(suffix);
            state.markKind(quad.arg(2), (width.float_ && !isLogicalOperator(base)) ? Kind::Float : Kind::Int);
        }
        /* 比较 / 逻辑运算即使不带后缀，结果也得是整数（`ne`、`and` 这种）。 */
        if (isLogicalOperator(base) && quad.args.size() >= 3u && state.kindOf(quad.arg(2)) != Kind::String)
            state.markKind(quad.arg(2), Kind::Int);
    }
    /* 4.5 字符串来源：只认**确实返回字符串**的那几个内建。
     * 不能按名字里含 `String` 来猜——`std::lang::String.length` 返回的是整数，
     * `__std_str_len` 也是，猜错就会把长度当字符串打印。 */
    for (std::size_t at = 0; at < quads.size(); ++at)
    {
        const auto &quad = quads[at];
        if (quad.op != "call" || quad.args.size() < 3u) continue;
        const Memory::String &callee = quad.arg(0);
        const Memory::String &target = quad.arg(2);
        if (target.empty() || target == "_" || !literalIsName(target)) continue;
        const auto external = state.externsByLabel.find(std::string(callee.c_str()));
        if (callee == "__std_read_float" || callee == "__std_input_float" || (external != state.externsByLabel.end() && (external->second->resultType == "double" || external->second->resultType == "float")))
            state.markKind(target, Kind::Float);
        if (callee == "std::lang::__std_str_of_flt" || callee == "std::lang::__bootStringCopy" || callee == "std::lang::__bootStringNew" || callee == "std::lang::__bootStringSlice" || callee == "std::lang::__bootStringFromInt" || callee.rfind("std::lang::__bootStringJoin", 0u) == 0u || callee == "__std_read_line" || callee == "__std_input_line" || callee == "std::lang::__readLine" || callee == "__readLine")
            state.markString(target);
    }
    // 原生服务和函数值返回原始位模式，由显式浮点消费者反推结果形态。
    std::set<std::string> floatingUses;
    for (const auto &function : state.functions)
    {
        std::vector<std::pair<Memory::String, Memory::String>> parameters;
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = quads[at];
            if (quad.op == "param") parameters.push_back({ quad.arg(0), quad.arg(2) });
            if (quad.op != "call" && quad.op != "callptr") continue;
            const auto found = state.functionByLabel.find(std::string(quad.arg(0).c_str()));
            if (found != state.functionByLabel.end())
            {
                const auto &formals = state.functions[found->second].parameters;
                for (std::size_t index = 0; index < formals.size() && index < parameters.size(); ++index)
                {
                    if (state.kindOf(formals[index]) != Kind::Float) continue;
                    auto actual = parameters[index].first;
                    for (const auto &parameter : parameters)
                        if (parameter.second == formals[index]) actual = parameter.first;
                    floatingUses.insert(std::string(actual.c_str()));
                }
            }
            parameters.clear();
        }
    }
    for (const auto &quad : quads)
    {
        Memory::String base, suffix;
        splitOperation(quad.op, base, suffix);
        if (suffix != "f32" && suffix != "f64") continue;
        if (base == "store")
            floatingUses.insert(std::string(quad.arg(1).c_str()));
        else if (base == "cvt")
        {
            if (quad.arg(1) == "f32" || quad.arg(1) == "f64") floatingUses.insert(std::string(quad.arg(0).c_str()));
        }
        else if (base != "load")
        {
            floatingUses.insert(std::string(quad.arg(0).c_str()));
            floatingUses.insert(std::string(quad.arg(1).c_str()));
        }
    }
    bool propagating = true;
    while (propagating)
    {
        propagating = false;
        for (const auto &quad : quads)
            if (quad.op == ":=" && (state.kindOf(quad.arg(2)) == Kind::Float || floatingUses.count(std::string(quad.arg(2).c_str()))))
                propagating |= floatingUses.insert(std::string(quad.arg(0).c_str())).second;
    }
    for (const auto &quad : quads)
        if ((quad.op == "call" || quad.op == "callptr") && floatingUses.count(std::string(quad.arg(2).c_str())))
            state.markKind(quad.arg(2), Kind::Float);

    /* 浮点与字符串沿着 `:=`、`load`、函数返回值扩散；顺带重算每个函数的返回种类。
     * 多跑几轮直到稳定——返回种类和调用点的种类是互相喂出来的。 */
    for (;;)
    {
        bool changed = false;
        for (auto &function : state.functions)
        {
            bool floating = false, string = false;
            for (std::size_t at = function.begin; at < function.end; ++at)
            {
                const auto &quad = quads[at];
                if (quad.op != "ret") continue;
                const auto kind = !literalIsName(quad.arg(0)) && literalIsFloat(quad.arg(0)) ? Kind::Float : state.kindOf(quad.arg(0));
                floating |= kind == Kind::Float;
                string |= kind == Kind::String;
            }
            changed |= function.returnsFloat != floating || function.returnsString != string;
            function.returnsFloat = floating;
            function.returnsString = string;
        }
        for (std::size_t at = 0; at < quads.size(); ++at)
        {
            const auto &quad = quads[at];
            const Memory::String op = quad.op;
            if (quad.args.size() < 3u) continue;
            const Memory::String &target = quad.arg(2);
            if (target.empty() || target == "_" || !literalIsName(target)) continue;
            /* 调用返回值：被调函数返回什么，接收方就是什么。 */
            if (op == "call")
            {
                const auto found = state.functionByLabel.find(std::string(quad.arg(0).c_str()));
                if (found == state.functionByLabel.end()) continue;
                const Function &callee = state.functions[found->second];
                const Kind kind = callee.returnsString ? Kind::String : (callee.returnsFloat ? Kind::Float : Kind::Int);
                if (kind != Kind::Int && state.kindOf(target) != kind)
                {
                    changed |= state.markKind(target, kind);
                }
                continue;
            }
            if (op != ":=" && op.rfind("load", 0) != 0u) continue;
            const Memory::String &source = quad.arg(0);
            if (op == ":=" && !literalIsName(source) && literalIsFloat(source))
            {
                if (state.kindOf(target) != Kind::Float)
                {
                    changed |= state.markKind(target, Kind::Float);
                }
                continue;
            }
            if (!literalIsName(source)) continue;
            const Kind kind = state.kindOf(source);
            if (kind == Kind::Int) continue;
            if (state.kindOf(target) != kind)
            {
                changed |= state.markKind(target, kind);
            }
        }
        if (!changed) break;
    }

    /* 6) 可达性：从可执行入口或库生命周期函数及导出函数出发沿 `call` 边走一遍。
     * 走不到的段（标准库里绝大多数）不翻译，保留分析签名。 */
    Memory::Vector<Memory::String> pending;
    if (!state.request.options.shared) pending.push_back("main");
    if (state.request.options.shared)
    {
        pending.push_back("__chali_library_init");
        pending.push_back("__chali_library_finish");
    }
    for (const auto &function : state.functions)
        if (function.exported) pending.push_back(function.label);
    std::set<std::string> liveFunctionValues;
    while (!pending.empty())
    {
        const Memory::String label = pending.back();
        pending.pop_back();
        if (!state.reachable.emplace(std::string(label.c_str())).second) continue;
        const auto found = state.functionByLabel.find(std::string(label.c_str()));
        if (found == state.functionByLabel.end()) continue;
        const Function &function = state.functions[found->second];
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = quads[at];
            Memory::String target;
            if (quad.op == "call") target = quad.arg(0);
            else if (quad.op == "addr")
            {
                const auto offset = state.offsetOf(quad.arg(0));
                const auto value = state.functionAddresses.find(static_cast<std::size_t>(offset));
                if (offset < 0 || value == state.functionAddresses.end()) continue;
                target = value->second;
                liveFunctionValues.insert(std::string(target.c_str()));
            }
            else continue;
            if (state.functionByLabel.count(std::string(target.c_str())) == 0u) continue;
            if (state.reachable.count(std::string(target.c_str())) != 0u) continue;
            pending.push_back(target);
        }
    }
    state.addressedFunctions = std::move(liveFunctionValues);
    for (auto at = state.functionAddresses.begin(); at != state.functionAddresses.end();)
        if (state.addressedFunctions.count(std::string(at->second.c_str())) == 0u) at = state.functionAddresses.erase(at);
        else ++at;
    for (const auto &label : state.reachable)
        if (label.rfind("std::watchdog::", 0u) == 0u) state.watchdogUsed = true;
    for (const auto &label : state.addressedFunctions) pending.push_back(Memory::String(label.c_str()));
    while (!pending.empty())
    {
        const auto label = pending.back();
        pending.pop_back();
        if (!state.callbackReachable.insert(std::string(label.c_str())).second) continue;
        const auto found = state.functionByLabel.find(std::string(label.c_str()));
        if (found == state.functionByLabel.end()) continue;
        const auto &function = state.functions[found->second];
        for (std::size_t at = function.begin; at < function.end; ++at)
            if (quads[at].op == "call") pending.push_back(quads[at].arg(0));
    }
}

/**
 * @brief 判断函数能否从主入口或显式导出入口到达。
 * @param state 已完成调用图分析的状态。
 * @param function 待判断的函数。
 * @return 函数被导出或可达时返回 true。
 */
bool isLive(const Translator &state, const Function &function)
{
    return function.exported || state.reachable.count(std::string(function.label.c_str())) != 0u;
}

/**
 * @brief 仅保存可达数据的非零片段，保持运行时逻辑地址与对象身份不变。
 * @param state 已完成函数可达性分析的翻译状态。
 * @param out 接收稀疏映像、布局及片段表的 LLVM 模块。
 */
void emitDataImage(const Translator &state, std::string &out)
{
    Memory::Vector<stackvm::LayoutRow> rows;
    if (!stackvm::decodeDataLayout(state.request.dataLayout, rows)) return;
    const auto *flag = std::getenv("CHALI_AOT_KEEP_DATA");
    const bool keepAll = state.request.syntax != nullptr || (flag && *flag && std::strcmp(flag, "0") != 0);
    std::set<Memory::String> used;
    for (const auto &function : state.functions)
        if (isLive(state, function))
            for (std::size_t at = function.begin; at < function.end; ++at)
            {
                const auto &quad = state.request.quads->quads[at];
                if (quad.op == "addr") used.insert(quad.arg(0));
            }
    const auto &original = state.request.dataBytes;
    Memory::String selected(original.size(), '\0');
    Memory::Vector<stackvm::LayoutRow> retained;
    for (const auto &row : rows)
    {
        // 缺少长度或范围不合法的外部布局无法证明可裁剪，整体保留原始请求。
        if (!row.hasSize || row.offset > original.size() || row.size > original.size() - row.offset)
        {
            selected = original;
            retained = rows;
            break;
        }
        bool functionAddress = false;
        for (const auto &entry : state.functionAddresses)
            if (entry.first >= row.offset && entry.first - row.offset < row.size) functionAddress = true;
        if (!keepAll && used.count(row.name) == 0u && !functionAddress) continue;
        retained.push_back(row);
        // 布局的地址指向数组首元素，前八字节可能是长度头；不重排或合并逻辑对象。
        const std::size_t begin = row.offset >= 8u ? row.offset - 8u : 0u;
        const std::size_t end = row.offset + row.size;
        selected.replace(begin, end - begin, original, begin, end - begin);
    }
    if (keepAll) selected = original;
    struct Segment
    {
        std::size_t destination, source, size;
    };
    std::vector<Segment> segments;
    Memory::String packed;
    std::map<Memory::String, std::size_t> payloads;
    for (std::size_t at = 0; at < selected.size();)
    {
        while (at < selected.size() && selected[at] == '\0') ++at;
        if (at == selected.size()) break;
        const std::size_t begin = at;
        std::size_t end = at;
        while (at < selected.size())
        {
            if (selected[at++] != '\0')
                end = at;
            else if (at - end >= 32u)
                break;
        }
        const auto payload = selected.substr(begin, end - begin);
        const auto inserted = payloads.emplace(payload, packed.size());
        if (inserted.second) packed += payload;
        segments.push_back({ begin, inserted.first->second, end - begin });
    }
    // 对照模式保留旧版完整映像，避免把稀疏编码的收益混入可达数据裁剪。
    if (keepAll)
    {
        packed = original;
        segments.clear();
        if (!original.empty()) segments.push_back({ 0u, 0u, original.size() });
    }
    const auto layout = stackvm::encodeDataLayout(retained);
    out += "@chali_data_layout_size = constant i64 " + std::to_string(layout.size()) + "\n";
    out += "; AOT data image: original=" + std::to_string(original.size()) + " stored=" + std::to_string(packed.size())
           + " rows=" + std::to_string(retained.size()) + "/" + std::to_string(rows.size()) + " segments=" + std::to_string(segments.size()) + "\n";
    out += "@chali_data_layout = constant [" + std::to_string(layout.size() + 1u) + " x i8] c\"";
    for (const unsigned char byte : layout)
    {
        char escaped[4];
        std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
        out += escaped;
    }
    out += "\\00\"\n@chali_data_image = constant [" + std::to_string(packed.size()) + " x i8] c\"";
    for (const unsigned char byte : packed)
    {
        char escaped[4];
        std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
        out += escaped;
    }
    out += "\"\n@chali_data_segment_count = constant i64 " + std::to_string(segments.size()) + "\n";
    out += "@chali_data_segments = constant [" + std::to_string(segments.size()) + " x { i64, i64, i64 }] [";
    for (std::size_t at = 0; at < segments.size(); ++at)
    {
        if (at) out += ", ";
        const auto &segment = segments[at];
        out += "{ i64, i64, i64 } { i64 " + std::to_string(segment.destination) + ", i64 " + std::to_string(segment.source)
               + ", i64 " + std::to_string(segment.size) + " }";
    }
    out += "]\n";
}

using Text = std::string;

/**
 * @brief 将项目字符串复制为生成器内部的标准字符串。
 * @param value 项目分配器持有的字符串。
 * @return 保持全部字节的副本。
 */
Text textOf(const Memory::String &value)
{
    return Text(value.data(), value.size());
}

/**
 * @brief 将外部符号编码为 LLVM 引用标识符。
 * @param symbol 外部链接器实际使用的符号字节。
 * @return 双引号包围并转义控制字节、反斜杠和引号的 LLVM 标识符。
 */
Text llvmSymbol(const Memory::String &symbol)
{
    Text result = "\"";
    for (const unsigned char byte : symbol)
    {
        if (byte >= 32 && byte < 127 && byte != '"' && byte != '\\')
            result += static_cast<char>(byte);
        else
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            result += escaped;
        }
    }
    return result + "\"";
}

/**
 * @brief LLVM 指令操作数及其精确类型。
 */
struct Value
{
    Text type;
    Text text;
    Text floating = {};  ///< 不透明整数槽所携带的运行时浮点标记。
};

/**
 * @brief 将 ABI 声明的 C 类型映射到 LLVM 类型。
 * @param name 前端提供的 C 类型拼写。
 * @return LLVM 标量或指针类型；不支持的类型返回空串。
 */
Text abiType(const Memory::String &name)
{
    if (name.find('*') != Memory::String::npos) return "ptr";
    if (name == "void") return "void";
    if (name == "double") return "double";
    if (name == "float") return "float";
    if (name.find("char") != Memory::String::npos || name == "bool") return "i8";
    if (name.find("short") != Memory::String::npos) return "i16";
    if (name.find("long long") != Memory::String::npos || name.find("int64") != Memory::String::npos) return "i64";
    if (name == "int" || name == "unsigned int") return "i32";
    return Text();
}

/**
 * @brief 将函数内的可变符号降低为内存槽，将四元式运算降低为 LLVM 指令。
 * @note 只有形参与本函数声明的局部使用 alloca，跨函数暂存量保持模块级共享。
 */
struct Builder
{
    Translator &state;
    const Function &function;
    Text body;
    std::set<Text> locals;
    std::map<Text, std::size_t> labels;
    std::map<std::size_t, std::size_t> logicalCosts;
    std::map<std::size_t, std::size_t> lineOccurrences;
    std::size_t serial = 0;
    std::vector<std::pair<Memory::String, Value>> arguments;
    bool pendingFormatValid = false;  ///< 紧随其后的 `__std_print` 调用是否带有可解析的编译期格式串。
    std::string pendingFormat;        ///< 编译期已知的格式串字节（仅字面量与 %s 时直连 stdout）。
    std::map<std::pair<Text, int>, Value> checkedAddresses;  ///< 地址符号及精确访问宽度对应的验证结果。
    std::map<Text, Memory::String> knownFunctions;           ///< 当前直线路径内尚未失效的函数值。
    std::map<Text, std::string> knownLiterals;               ///< 当前直线路径内已知的字符串字面量。
    std::map<Text, Value> astValues;                        ///< AST 表达式的 SSA 值，不分配帧槽。

    /**
     * @brief 建立函数的帧槽集合及本函数内的跳转标签表。
     * @param translation 当前编译请求的分析结果。
     * @param current 当前函数的范围与签名。
     */
    Builder(Translator &translation, const Function &current) : state(translation), function(current)
    {
        for (const auto &parameter : function.parameters) locals.insert(textOf(parameter));
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = state.request.quads->quads[at];
            if (quad.op == "decl" && quad.arg(1) != "global" && quad.arg(2).rfind("__temp_", 0) != 0u && quad.arg(2).rfind("__chali_ret_v$", 0) != 0u) locals.insert(textOf(quad.arg(2)));
            if (quad.op == "label")
                labels[textOf(quad.arg(2))] = at;
            else if (state.watchdogUsed)
                ++lineOccurrences[quad.line];
        }
        if (state.watchdogUsed && state.request.program)
        {
            const auto &program = *state.request.program;
            const auto entry = program.labels.find(program.symbolKey(function.label));
            std::size_t end = program.code.size();
            const auto index = state.functionByLabel.at(textOf(function.label));
            if (index + 1 < state.functions.size())
            {
                const auto next = program.labels.find(program.symbolKey(state.functions[index + 1].label));
                if (next != program.labels.end()) end = next->second;
            }
            if (entry != program.labels.end())
                for (auto pc = entry->second; pc < end; ++pc)
                    if (const auto *location = program.debug.lookup(pc)) ++logicalCosts[location->quadLine];
        }
    }

    /**
     * @brief 分配当前函数内唯一的 SSA 名称。
     * @return 不与形参或基本块冲突的 LLVM 局部名称。
     */
    Text fresh()
    {
        return "%r" + std::to_string(serial++);
    }

    /**
     * @brief 根据推断结果取得符号的统一存储类型。
     * @param token 四元式符号或立即数。
     * @return 浮点返回 double，其余值返回 i64。
     */
    Text type(const Memory::String &token)
    {
        return state.kindOf(token) == Kind::Float || (!literalIsName(token) && literalIsFloat(token)) ? "double" : "i64";
    }

    /**
     * @brief 取得符号对应的局部槽或模块级共享槽。
     * @param token 四元式符号名称。
     * @return 可用于 load/store 的 LLVM 指针操作数。
     */
    Text pointer(const Memory::String &token)
    {
        return (locals.count(textOf(token)) != 0u ? "%s_" : "@s_") + textOf(state.use(token));
    }

    /**
     * @brief 取得整数槽的动态类型标记地址，作用域与值槽保持一致。
     * @param token 四元式符号。
     * @return 本帧或模块共享的单字节标记地址。
     */
    Text tagPointer(const Memory::String &token)
    {
        return (locals.count(textOf(token)) != 0u ? "%t_" : "@t_") + textOf(state.use(token));
    }

    /**
     * @brief 写入一条有结果的 LLVM 指令。
     * @param resultType 指令结果的 LLVM 类型。
     * @param instruction 不含赋值部分的 LLVM 指令。
     * @return 对应的 SSA 值。
     */
    Value instruction(const Text &resultType, const Text &instruction)
    {
        const Text name = fresh();
        body += "  " + name + " = " + instruction + "\n";
        return { resultType, name };
    }

    /**
     * @brief 在 LLVM 标量与宿主指针类型之间显式转换。
     * @param value 源值。
     * @param target 目标 LLVM 类型。
     * @param unsignedValue 整数转换是否按无符号解释。
     * @return 转换后的 SSA 值或无需转换的源值。
     */
    Value convert(Value value, const Text &target, bool unsignedValue = false)
    {
        if (value.type == target) return value;
        if (value.type == "i64" && !value.floating.empty() && target == "float")
            return convert(convert(value, "double", unsignedValue), "float", unsignedValue);
        if (value.type == "i64" && !value.floating.empty() && target == "double")
        {
            const auto real = instruction("double", "bitcast i64 " + value.text + " to double");
            const auto integer = instruction("double", Text(unsignedValue ? "uitofp" : "sitofp") + " i64 " + value.text + " to double");
            const auto flag = instruction("i1", "icmp ne i8 " + value.floating + ", 0");
            return instruction("double", "select i1 " + flag.text + ", double " + real.text + ", double " + integer.text);
        }
        const bool sourceFloat = value.type == "double" || value.type == "float";
        const bool targetFloat = target == "double" || target == "float";
        Text operation;
        if (value.type == "ptr")
            operation = "ptrtoint";
        else if (target == "ptr")
            operation = "inttoptr";
        else if (sourceFloat && targetFloat)
            operation = target == "double" ? "fpext" : "fptrunc";
        else if (sourceFloat)
            operation = unsignedValue ? "fptoui" : "fptosi";
        else if (targetFloat)
            operation = unsignedValue ? "uitofp" : "sitofp";
        else
        {
            const int from = std::atoi(value.type.c_str() + 1), to = std::atoi(target.c_str() + 1);
            operation = from > to ? "trunc" : (unsignedValue || from == 1 ? "zext" : "sext");
        }
        return instruction(target, operation + " " + value.type + " " + value.text + " to " + target);
    }

    /**
     * @brief 捕获立即数或符号在当前位置的值。
     * @param token 四元式操作数；空值和占位符按零处理。
     * @return 带类型的立即数或 load 结果。
     */
    Value read(const Memory::String &token)
    {
        const auto expression = astValues.find(textOf(token));
        if (expression != astValues.end()) return expression->second;
        if (token.empty() || token == "_") return { "i64", "0" };
        if (!literalIsName(token))
        {
            if (literalIsFloat(token))
            {
                const double value = std::strtod(token.c_str(), nullptr);
                std::uint64_t bits = 0;
                std::memcpy(&bits, &value, sizeof(bits));
                char buffer[32];
                std::snprintf(buffer, sizeof(buffer), "0x%016llX", static_cast<unsigned long long>(bits));
                return { "double", buffer };
            }
            return { "i64", textOf(token) };
        }
        const Text storageType = type(token);
        auto value = instruction(storageType, "load " + storageType + ", ptr " + pointer(token));
        if (storageType == "i64") value.floating = instruction("i8", "load i8, ptr " + tagPointer(token)).text;
        return value;
    }

    /**
     * @brief 将结果存入四元式指定的符号。
     * @param token 目标符号；空值或占位符不写入。
     * @param value 待保存的值。
     */
    void write(const Memory::String &token, Value value)
    {
        if (token.rfind("__llvm_value_", 0u) == 0u)
        {
            astValues[textOf(token)] = convert(value, type(token));
            return;
        }
        knownFunctions.erase(textOf(token));
        knownLiterals.erase(textOf(token));
        for (auto it = checkedAddresses.begin(); it != checkedAddresses.end();)
            if (it->first.first == textOf(token))
                it = checkedAddresses.erase(it);
            else
                ++it;
        if (token.empty() || token == "_" || value.type == "void") return;
        value = convert(value, type(token));
        body += "  store " + value.type + " " + value.text + ", ptr " + pointer(token) + "\n";
        if (value.type == "i64") body += "  store i8 " + (value.floating.empty() ? Text("0") : value.floating) + ", ptr " + tagPointer(token) + "\n";
    }

    /**
     * @brief 生成显式类型的函数调用。
     * @param name LLVM 函数符号，不含 @。
     * @param resultType 返回类型。
     * @param values 已完成类型转换的实际参数。
     * @return 调用返回的 SSA 值；void 调用返回无结果标记。
     */
    Value call(const Text &name, const Text &resultType, const std::vector<Value> &values)
    {
        Text operation = "call " + resultType + " @" + name + "(";
        for (std::size_t index = 0; index < values.size(); ++index)
        {
            if (index != 0u) operation += ", ";
            operation += values[index].type + " " + values[index].text;
        }
        operation += ")";
        if (resultType != "void") return instruction(resultType, operation);
        body += "  " + operation + "\n";
        return { "void", "" };
    }

    /**
     * @brief 获取已经在 param 处捕获的实参。
     * @param index 缺少名称匹配时使用的参数位置。
     * @param formal 优先匹配的形参名。
     * @return 找到的实参，缺失时为整数零。
     */
    Value argument(std::size_t index, const Memory::String &formal = Memory::String())
    {
        if (!formal.empty())
            for (const auto &item : arguments)
                if (item.first == formal) return item.second;
        return index < arguments.size() ? arguments[index].second : Value{ "i64", "0" };
    }

    /**
     * @brief 根据形参协议降低内建、用户函数和外部 C ABI 调用。
     * @param callee 被调用的四元式标签。
     * @param destination 接收返回值的四元式符号。
     * @return 支持并成功生成时返回 true；否则记录具体诊断。
     */
    bool emitCall(const Memory::String &callee, const Memory::String &destination)
    {
        // 普通双精度数学操作直接进入 LLVM；不使用 fast-math，保留 NaN、无穷及负零。
        if (state.rawIntrinsics && !state.watchdogUsed)
        {
            static const std::map<Text, Text> unaryMath{
                { "std::math::__sqrt64", "llvm.sqrt.f64" },
                { "std::math::__floor64", "llvm.floor.f64" },
                { "std::math::__ceil64", "llvm.ceil.f64" },
                { "std::math::__round64", "llvm.round.f64" },
                { "std::math::__trunc64", "llvm.trunc.f64" }
            };
            const auto unary = unaryMath.find(textOf(callee));
            if (unary != unaryMath.end() && arguments.size() == 1u)
            {
                write(destination, call(unary->second, "double", { convert(argument(0), "double") }));
                return true;
            }
            if (callee == "std::math::__fmod64" && arguments.size() == 2u)
            {
                const auto left = convert(argument(0), "double");
                const auto right = convert(argument(1), "double");
                write(destination, instruction("double", "frem double " + left.text + ", " + right.text));
                return true;
            }
            if (arguments.size() == 1u && (callee == "std::math::__isNaN64" || callee == "std::math::__isInf64"))
            {
                const auto value = convert(argument(0), "double");
                Value result;
                if (callee == "std::math::__isNaN64")
                    result = instruction("i1", "fcmp uno double " + value.text + ", " + value.text);
                else
                {
                    const auto bits = instruction("i64", "bitcast double " + value.text + " to i64");
                    const auto magnitude = instruction("i64", "and i64 " + bits.text + ", 9223372036854775807");
                    result = instruction("i1", "icmp eq i64 " + magnitude.text + ", 9218868437227405312");
                }
                write(destination, convert(result, "i64", true));
                return true;
            }
        }
        if (state.rawIntrinsics && !state.watchdogUsed && arguments.size() == 1u
            && (callee == "std::lang::__stringStorageLength" || callee == "std::lang::__stringStorageData"))
        {
            write(destination, call("chali_host_string_storage", "i64",
                                    { { "i64", callee == "std::lang::__stringStorageLength" ? "0" : "1" }, convert(argument(0), "i64") }));
            return true;
        }
        // 核心 IEEE 编解码的局部重解释无需真正建立引用存储；其存活及范围静态可证。
        const bool float64Bits = callee == "std::lang::__bootFloat64Bits";
        const bool float32Bits = callee == "std::lang::__bootFloat32Bits";
        const bool float64Value = callee == "std::lang::__bootFloat64FromBits";
        const bool float32Value = callee == "std::lang::__bootFloat32FromBits";
        if (state.rawIntrinsics && !state.watchdogUsed && arguments.size() == 1u
            && (float64Bits || float32Bits || float64Value || float32Value))
        {
            Value result;
            if (float64Bits || float32Bits)
            {
                const auto real = convert(argument(0), float32Bits ? "float" : "double");
                result = instruction(float32Bits ? "i32" : "i64", "bitcast " + real.type + " " + real.text
                                                                      + " to " + (float32Bits ? "i32" : "i64"));
                result = convert(result, "i64", true);
            }
            else
            {
                const auto bits = convert(argument(0), float32Value ? "i32" : "i64", true);
                result = instruction(float32Value ? "float" : "double", "bitcast " + bits.type + " " + bits.text
                                                                            + " to " + (float32Value ? "float" : "double"));
                result = convert(result, "double");
            }
            write(destination, result);
            return true;
        }
        static const std::map<Text, std::pair<int, std::size_t>> listStorageCalls = {
            { "std::lang::__listStorageGuard", { 0, 3 } },
            { "std::lang::__listStorageCapacity", { 1, 1 } },
            { "std::lang::__listStorageReserve", { 2, 2 } },
            { "std::lang::__listStorageSwap", { 3, 3 } },
            { "std::lang::__listStorageDiscard", { 4, 1 } },
            { "std::lang::__listStorageEqualI", { 5, 5 } },
            { "std::lang::__listStorageEqualF", { 5, 5 } },
            { "std::lang::__listStorageCloneAppend", { 6, 3 } },
            { "std::lang::__listStorageAppendI", { 7, 4 } },
            { "std::lang::__listStorageAppendF", { 7, 4 } },
            { "std::lang::__listStorageTakeI", { 8, 3 } },
            { "std::lang::__listStorageTakeF", { 8, 3 } },
            { "std::lang::__listStorageNew", { 9, 0 } },
            { "std::lang::__listStorageLength", { 10, 1 } },
            { "__std_vec_new", { 9, 0 } },
            { "__std_vec_len", { 10, 1 } },
            { "__std_vec_at", { 11, 4 } },
            { "__std_vec_set", { 12, 5 } },
            { "__std_iter_lease", { 13, 1 } },
            { "std::lang::__listStorageCompareI", { 14, 5 } },
            { "std::lang::__listStorageCompareF", { 14, 5 } },
            { "std::lang::__listStorageCompareIndex", { 15, 3 } },
            { "std::lang::__listStorageReadI", { 11, 4 } },
            { "std::lang::__listStorageReadF", { 11, 4 } },
            { "std::lang::__listStorageCloneI", { 16, 3 } },
            { "std::lang::__listStorageCloneF", { 16, 3 } }
        };
        const auto listStorage = listStorageCalls.find(textOf(callee));
        static const std::map<Text, std::pair<int, std::size_t>> mapStorageCalls = {
            { "std::lang::__mapStorageState", { 0, 4 } },
            { "std::lang::__mapStorageHashI", { 1, 4 } },
            { "std::lang::__mapStorageHashF", { 1, 4 } },
            { "std::lang::__mapStorageMatchI", { 2, 5 } },
            { "std::lang::__mapStorageMatchF", { 2, 5 } },
            { "std::lang::__mapStoragePrepareII", { 3, 7 } },
            { "std::lang::__mapStoragePrepareIF", { 3, 7 } },
            { "std::lang::__mapStoragePrepareFI", { 3, 7 } },
            { "std::lang::__mapStoragePrepareFF", { 3, 7 } },
            { "std::lang::__mapStorageCommit", { 4, 3 } },
            { "std::lang::__setStorageAppendI", { 5, 5 } },
            { "std::lang::__setStorageAppendF", { 5, 5 } },
            { "std::lang::__mapStorageErase", { 6, 2 } },
            { "std::lang::__mapStorageReadI", { 7, 4 } },
            { "std::lang::__mapStorageReadF", { 7, 4 } },
            { "std::lang::__mapStorageEmptyLike", { 8, 1 } },
            { "std::lang::__mapStorageCloneAppend", { 9, 3 } },
            { "std::lang::__setStoragePrepareI", { 10, 4 } },
            { "std::lang::__setStoragePrepareF", { 10, 4 } },
            { "std::lang::__setStorageCancel", { 11, 1 } },
            { "__std_map_new", { 12, 0 } },
            { "__std_set_new", { 13, 0 } },
            { "__std_map_new_policy", { 14, 2 } },
            { "__std_set_new_policy", { 15, 2 } },
            { "__std_map_len", { 16, 1 } },
            { "__std_set_len", { 17, 1 } },
            { "__std_map_key_at", { 18, 4 } },
            { "__std_map_val_at", { 19, 4 } },
            { "__std_set_at", { 20, 4 } }
        };
        const auto mapStorage = mapStorageCalls.find(textOf(callee));
        const auto *storage = listStorage != listStorageCalls.end() ? &listStorage->second :
                              mapStorage != mapStorageCalls.end()   ? &mapStorage->second :
                                                                      nullptr;
        const bool hashed = listStorage == listStorageCalls.end();
        if (state.rawIntrinsics && !state.watchdogUsed && storage && arguments.size() == storage->second)
        {
            std::vector<std::pair<Memory::String, Value>> saved;
            for (const auto &entry : state.temporaryUsers)
            {
                if (!state.callbackReachable.count(textOf(function.label))) break;
                if (entry.second.size() != 1u || entry.second.count(textOf(function.label)) == 0u || locals.count(entry.first)) continue;
                const Memory::String token(entry.first.c_str());
                saved.push_back({ token, read(token) });
            }
            std::vector<Value> values{ { "i64", std::to_string(storage->first) } };
            unsigned floating = 0;
            Value dynamicFloating{ "i64", "0" };
            for (std::size_t i = 0; i < (hashed ? 7u : 5u); ++i)
            {
                Value value = i < arguments.size() ? arguments[i].second : Value{ "i64", "0" };
                if (value.type == "double")
                {
                    value = instruction("i64", "bitcast double " + value.text + " to i64");
                    floating |= 1u << i;
                }
                else if (!value.floating.empty())
                {
                    const auto flag = instruction("i64", "zext i8 " + value.floating + " to i64");
                    const auto shifted = instruction("i64", "shl i64 " + flag.text + ", " + std::to_string(i));
                    dynamicFloating = instruction("i64", "or i64 " + dynamicFloating.text + ", " + shifted.text);
                }
                values.push_back(convert(value, "i64"));
            }
            values.push_back(instruction("i64", "or i64 " + dynamicFloating.text + ", " + std::to_string(floating)));
            Value result = call(hashed ? "chali_host_map_storage" : "chali_host_list_storage", "i64", values);
            result.floating = call("chali_host_result_float", "i8", {}).text;
            for (const auto &entry : saved) write(entry.first, entry.second);
            if (type(destination) == "double") result = instruction("double", "bitcast i64 " + result.text + " to double");
            write(destination, result);
            return true;
        }
        if (state.rawIntrinsics && !state.watchdogUsed && callee == "std::lang::__rawStorage" && arguments.size() == 6u)
        {
            std::vector<Value> values;
            for (const auto &item : arguments) values.push_back(convert(item.second, "i64"));
            write(destination, call("chali_host_raw_storage", "i64", values));
            return true;
        }
        /* 标准错误流直连：字符串 data/len 取自运行时，再直接 fwrite 到 stderr，不经宿主胶水。 */
        if (state.rawIntrinsics && !state.watchdogUsed && callee == "std::lang::__std_stderr_write" && arguments.size() == 1u)
        {
            const Value handle = convert(argument(0), "i64");
            const auto length = call("chali_host_string_storage", "i64", { { "i64", "0" }, handle });
            const auto content = call("chali_host_string_storage", "i64", { { "i64", "1" }, handle });
            const auto data = call("chali_host_address", "ptr", { content, length });
            const auto stream = call("chali_standard_stream", "ptr", { { "i32", "2" } });
            const auto written = call("fwrite", "i64", { data, { "i64", "1" }, length, stream });
            const auto flushed = call("fflush", "i32", { stream });
            const auto shortWrite = instruction("i1", "icmp ne i64 " + written.text + ", " + length.text);
            const auto flushFailed = instruction("i1", "icmp ne i32 " + flushed.text + ", 0");
            const auto failed = instruction("i1", "or i1 " + shortWrite.text + ", " + flushFailed.text);
            const auto failure = "stderr_failed_" + std::to_string(serial++);
            const auto done = "stderr_done_" + std::to_string(serial++);
            const auto message = "@chali_abi_lit" + std::to_string(state.printLiterals.size());
            state.printLiterals.push_back(std::string("标准错误流写入失败") + '\0');
            body += "  br i1 " + failed.text + ", label %" + failure + ", label %" + done + "\n" + failure + ":\n";
            call("chali_panic", "void", { { "ptr", message } });
            body += "  unreachable\n" + done + ":\n";
            write(destination, { "i64", "0" });
            return true;
        }
        /* 浮点定长格式化直连：调用模块级辅助函数完成 snprintf 与新字符串分配。 */
        if (state.rawIntrinsics && !state.watchdogUsed && callee == "std::lang::__std_str_fixed" && arguments.size() == 2u)
        {
            write(destination, call("chali_string_fixed", "i64", { convert(argument(0), "double"), convert(argument(1), "i64") }));
            return true;
        }
        /* 标准输入直连：读取一行直接调用运行时 IR 里的 chali_read_line（fgets + 去 LF/CRLF，EOF 返回空串）。 */
        if (state.rawIntrinsics && !state.watchdogUsed && (callee == "std::lang::__readLine" || callee == "__readLine" || callee == "__std_input_line") && arguments.empty())
        {
            write(destination, call("chali_read_line", "i64", {}));
            return true;
        }
        if (state.rawIntrinsics && !state.watchdogUsed && callee == "std::lang::__stdinEof" && arguments.empty())
        {
            write(destination, call("chali_stdin_eof", "i64", {}));
            return true;
        }
        /* 标准输出直连：编译期格式串只含字面量与 %s（花括号风格的字符串排版）时，
         * 逐段直接 fwrite 到 stdout，不再经过宿主格式化胶水；其余格式回落胶水。 */
        if (state.rawIntrinsics && !state.watchdogUsed && pendingFormatValid && callee == "__std_print")
        {
            std::vector<std::pair<bool, std::string>> steps;  ///< (是否取字符串实参, 字面量字节)。
            std::string literalRun;
            bool supported = true;
            for (std::size_t at = 0; at < pendingFormat.size() && supported;)
            {
                const char character = pendingFormat[at];
                if (character != '%')
                {
                    literalRun += character;
                    ++at;
                    continue;
                }
                if (at + 1u >= pendingFormat.size())
                {
                    supported = false;
                    break;
                }
                const char specifier = pendingFormat[at + 1u];
                if (specifier == '%')
                {
                    literalRun += '%';
                    at += 2u;
                    continue;
                }
                if (specifier == 's')
                {
                    if (!literalRun.empty())
                    {
                        steps.emplace_back(false, literalRun);
                        literalRun.clear();
                    }
                    steps.emplace_back(true, std::string());
                    at += 2u;
                    continue;
                }
                supported = false;
            }
            if (!literalRun.empty()) steps.emplace_back(false, literalRun);
            std::size_t stringCount = 0;
            for (const auto &step : steps)
                if (step.first) ++stringCount;
            if (!supported || stringCount + 1u != arguments.size())
            {
                pendingFormatValid = false; /* 含数值规格或无法解析：交给宿主格式化胶水。 */
            }
            else
            {
                const auto stream = call("chali_standard_stream", "ptr", { { "i32", "1" } });
                std::size_t stringIndex = 0;
                for (const auto &step : steps)
                {
                    if (!step.first)
                    {
                        const std::string name = "@chali_abi_lit" + std::to_string(state.printLiterals.size());
                        state.printLiterals.push_back(step.second);
                        const auto bytes = instruction("ptr", "getelementptr [" + std::to_string(step.second.size())
                                                                          + " x i8], ptr " + name + ", i64 0, i64 0");
                        call("fwrite", "i64", { bytes, { "i64", "1" }, { "i64", std::to_string(step.second.size()) }, stream });
                        continue;
                    }
                    const Value handle = convert(argument(1 + stringIndex++), "i64");
                    const auto length = call("chali_host_string_storage", "i64", { { "i64", "0" }, handle });
                    const auto content = call("chali_host_string_storage", "i64", { { "i64", "1" }, handle });
                    const auto data = call("chali_host_address", "ptr", { content, length });
                    call("fwrite", "i64", { data, { "i64", "1" }, length, stream });
                }
                write(destination, { "i64", "0" });
                pendingFormatValid = false;
                return true;
            }
        }
        const auto external = state.externsByLabel.find(textOf(callee));
        if (external != state.externsByLabel.end() && state.request.abiGlue.empty())
        {
            state.usedExternLabels.insert(textOf(callee));
            const auto &signature = *external->second;
            if (arguments.size() != signature.parameterTypes.size())
            {
                state.fail("AOT：外部函数实参数量不匹配：" + callee);
                return false;
            }
            std::vector<Value> values;
            for (std::size_t at = 0; at < signature.parameterTypes.size(); ++at)
            {
                const Text target = abiType(signature.parameterTypes[at]);
                if (target.empty())
                {
                    state.fail("AOT：不支持的 ABI 参数类型 " + signature.parameterTypes[at]);
                    return false;
                }
                values.push_back(convert(argument(at), target, signature.parameterTypes[at].find("unsigned") != Memory::String::npos));
            }
            const Text resultType = abiType(signature.resultType);
            if (resultType.empty())
            {
                state.fail("AOT：不支持的 ABI 返回类型 " + signature.resultType);
                return false;
            }
            Value result = call(llvmSymbol(signature.symbol), resultType, values);
            if (resultType == "void") result = { "i64", "0" };
            if (!resultType.empty() && resultType.front() == 'i')
                result = convert(result, "i64", signature.resultType.find("unsigned") != Memory::String::npos);
            write(destination, result);
            return true;
        }
        if (callee == "__chali_copy_struct" && state.bulkCopies && !state.watchdogUsed && arguments.size() == 3u)
        {
            write(destination, call("chali_host_copy_words", "i64", { convert(argument(0), "i64"), convert(argument(1), "i64"), convert(argument(2), "i64") }));
            return true;
        }
        const auto found = state.functionByLabel.find(textOf(callee));
        if (found == state.functionByLabel.end())
        {
            static const std::map<Text, Text> memoryHelpers = {
                { "__chali_new_array", "chali_new_array" },
                { "__chali_memcpy", "chali_copy_bytes" },
                { "__chali_memset", "chali_set_bytes" }
            };
            const auto helper = memoryHelpers.find(textOf(callee));
            if (helper != memoryHelpers.end())
            {
                std::vector<Value> values;
                for (const auto &item : arguments) values.push_back(convert(item.second, "i64"));
                write(destination, call(helper->second, "i64", values));
                return true;
            }
            const auto native = state.nativeNames.emplace(textOf(callee), state.nativeNames.size()).first->second;
            for (std::size_t index = 0; index < arguments.size(); ++index)
            {
                Value value = arguments[index].second;
                const bool floating = value.type == "double";
                if (floating) value = instruction("i64", "bitcast double " + value.text + " to i64");
                const auto slot = instruction("ptr", "getelementptr i64, ptr %indirect_arguments, i64 " + std::to_string(index));
                body += "  store i64 " + convert(value, "i64").text + ", ptr " + slot.text + "\n";
                const auto flag = instruction("ptr", "getelementptr i8, ptr %native_argument_types, i64 " + std::to_string(index));
                body += "  store i8 " + (floating ? Text("1") : (value.floating.empty() ? Text("0") : value.floating)) + ", ptr " + flag.text + "\n";
            }
            std::vector<std::pair<Memory::String, Value>> saved;
            for (const auto &entry : state.temporaryUsers)
            {
                if (!state.callbackReachable.count(textOf(function.label))) break;
                if (entry.second.size() != 1u || entry.second.count(textOf(function.label)) == 0u || locals.count(entry.first)) continue;
                const Memory::String token(entry.first.c_str());
                saved.push_back({ token, read(token) });
            }
            Value result = call("chali_host_call", "i64", { { "ptr", "@native_name" + std::to_string(native) }, { "i64", std::to_string(arguments.size()) }, { "ptr", "%indirect_arguments" }, { "ptr", "%native_argument_types" } });
            result.floating = call("chali_host_result_float", "i8", {}).text;
            for (const auto &entry : saved) write(entry.first, entry.second);
            if (type(destination) == "double") result = instruction("double", "bitcast i64 " + result.text + " to double");
            write(destination, result);
            return true;
        }
        const Function &target = state.functions[found->second];
        if (arguments.size() != target.parameters.size())
        {
            state.fail("AOT：函数实参数量不匹配：" + callee);
            return false;
        }
        std::vector<Value> values;
        for (std::size_t at = 0; at < target.parameters.size(); ++at) values.push_back(convert(argument(at, target.parameters[at]), type(target.parameters[at])));
        // 私有计算暂存通过 SSA 跨调用保存，不分配帧槽。跨函数共用的暂存始终读写模块共享表。
        std::vector<std::pair<Memory::String, Value>> saved;
        std::vector<Text> pending{ textOf(target.label) };
        std::set<Text> visited;
        bool reenters = false;
        while (!pending.empty() && !reenters)
        {
            const Text label = pending.back();
            pending.pop_back();
            if (label == textOf(function.label))
            {
                reenters = true;
                break;
            }
            if (!visited.insert(label).second) continue;
            const auto next = state.functionByLabel.find(label);
            if (next == state.functionByLabel.end())
            {
                reenters = state.callbackReachable.count(textOf(function.label)) != 0;
                continue;
            }
            const Function &reachable = state.functions[next->second];
            for (std::size_t at = reachable.begin; at < reachable.end; ++at)
            {
                const auto &quad = state.request.quads->quads[at];
                if (quad.op == "call") pending.push_back(textOf(quad.arg(0)));
                if (quad.op == "callptr") reenters |= state.callbackReachable.count(textOf(function.label)) != 0;
            }
        }
        for (const auto &entry : state.temporaryUsers)
        {
            if (!reenters) break;
            if (entry.second.size() != 1u || entry.second.count(textOf(function.label)) == 0u) continue;
            const Memory::String token(entry.first.c_str());
            if (locals.count(entry.first) == 0u) saved.push_back({ token, read(token) });
        }
        Value result = call(textOf(target.llvmName), target.returnsFloat ? "double" : "i64", values);
        if (!target.returnsFloat) result.floating = instruction("i8", "load i8, ptr @chali_return_float").text;
        for (const auto &entry : saved) write(entry.first, entry.second);
        write(destination, result);
        return true;
    }

    /**
     * @brief 生成算术、比较与逻辑运算并按四元式位宽收口。
     * @param base 去掉类型后缀后的操作符。
     * @param suffix 四元式类型后缀。
     * @param leftToken 左操作数。
     * @param rightToken 右操作数。
     * @param destination 结果符号。
     * @return 已识别该运算时返回 true，否则由上层报告不支持。
     */
    bool arithmetic(const Memory::String &base, const Memory::String &suffix, const Memory::String &leftToken, const Memory::String &rightToken, const Memory::String &destination)
    {
        static const std::map<Text, Text> operations = { { "+", "add" }, { "add", "add" }, { "-", "sub" }, { "sub", "sub" }, { "*", "mul" }, { "mul", "mul" }, { "/", "div" }, { "div", "div" }, { "%", "rem" }, { "mod", "rem" }, { "band", "and" }, { "&", "and" }, { "bor", "or" }, { "|", "or" }, { "bxor", "xor" }, { "^", "xor" }, { "shl", "shl" }, { "<<", "shl" }, { "shr", "ashr" }, { ">>", "ashr" }, { "shru", "lshr" }, { ">>>", "lshr" } };
        static const std::map<Text, Text> comparisons = { { "eq", "eq" }, { "==", "eq" }, { "ne", "ne" }, { "!=", "ne" }, { "lt", "lt" }, { "<", "lt" }, { "le", "le" }, { "<=", "le" }, { "gt", "gt" }, { ">", "gt" }, { "ge", "ge" }, { ">=", "ge" } };
        const Text name = textOf(base);
        const auto op = operations.find(name), comparison = comparisons.find(name);
        if (op == operations.end() && comparison == comparisons.end() && base != "neg" && base != "not" && base != "and" && base != "or") return false;
        const Width width = widthOf(suffix);
        Value left = read(leftToken), right = read(rightToken);
        const bool floating = width.float_ || left.type == "double" || right.type == "double";
        const Text target = floating ? (width.float_ && width.bits == 32 ? "float" : "double") : "i64";
        left = convert(left, target, width.unsigned_);
        right = convert(right, target, width.unsigned_);
        Value result;
        if (comparison != comparisons.end())
        {
            Text predicate = comparison->second;
            if (floating)
                predicate = predicate == "ne" ? "une" : "o" + predicate;
            else if (predicate != "eq" && predicate != "ne")
                predicate = (width.unsigned_ ? "u" : "s") + predicate;
            result = instruction("i1", Text(floating ? "fcmp " : "icmp ") + predicate + " " + target + " " + left.text + ", " + right.text);
        }
        else if (base == "and" || base == "or")
        {
            const Text zero = floating ? "0.0" : "0";
            const Text test = floating ? "fcmp une " : "icmp ne ";
            left = instruction("i1", test + target + " " + left.text + ", " + zero);
            right = instruction("i1", test + target + " " + right.text + ", " + zero);
            result = instruction("i1", name + " i1 " + left.text + ", " + right.text);
        }
        else if (base == "neg")
            result = instruction(target, floating ? "fneg " + target + " " + left.text : "sub " + target + " 0, " + left.text);
        else if (base == "not")
            result = instruction(target, "xor " + target + " " + left.text + ", -1");
        else
        {
            Text operation = op->second;
            if (floating)
                operation = "f" + operation;
            else if (operation == "div" || operation == "rem")
                operation = (width.unsigned_ ? "u" : "s") + operation;
            else if (operation == "ashr" && width.unsigned_)
                operation = "lshr";
            if (operation == "shl" || operation == "ashr" || operation == "lshr")
                call("chali_check_shift", "void", { right });
            if (operation == "sdiv" || operation == "udiv" || operation == "srem" || operation == "urem")
            {
                result = call("chali_integer_divide", "i64", { left, right, { "i32", width.unsigned_ ? "1" : "0" }, { "i32", operation.find("rem") != Text::npos ? "1" : "0" } });
            }
            else
            {
                if (operation == "fdiv" || operation == "frem") call("chali_check_real_divisor", "void", { convert(right, "double") });
                result = instruction(target, operation + " " + target + " " + left.text + ", " + right.text);
            }
        }
        if (!floating && result.type != "i1" && width.bits < 64)
            result = convert(convert(result, "i" + std::to_string(width.bits)), "i64", width.unsigned_);
        if (result.type != "i1") result = convert(result, floating ? "double" : "i64", width.unsigned_);
        write(destination, result);
        return true;
    }

    /**
     * @brief 生成当前函数的 LLVM 定义，逐个四元式构造显式基本块。
     * @return LLVM 函数文本；不支持的构造返回空串并登记诊断。
     */
    Text generate()
    {
        const Text resultType = function.returnsFloat ? "double" : "i64";
        Text signature = "define ";
        const bool typedExport = std::any_of(state.request.abiExports.begin(), state.request.abiExports.end(), [&](const auto &entry) { return entry.label == function.label; });
        if (state.target.windows && function.exported && !typedExport) signature += "dllexport ";
        if (!function.exported) signature += "internal ";
        signature += resultType + " @" + textOf(function.llvmName) + "(";
        for (std::size_t at = 0; at < function.parameters.size(); ++at)
        {
            if (at != 0u) signature += ", ";
            signature += type(function.parameters[at]) + " %p" + std::to_string(at);
        }
        signature += ") ";
        const Text rawLabel = textOf(function.label);
        Text pointerLabel = rawLabel;
        const Text destinationPrefix = "__chali_into$";
        if (pointerLabel.rfind(destinationPrefix, 0u) == 0u)
        {
            const auto separator = pointerLabel.find('$', destinationPrefix.size());
            if (separator != Text::npos) pointerLabel = pointerLabel.substr(separator + 1u);
        }
        const bool rawPointerWrapper = pointerLabel.rfind("std::lang::dereference__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::barePointer__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::pointerFromAddress__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__pointerCast__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::readAt__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::borrowAt__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::initAt__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::takeAt__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::memcpy__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::memset__", 0u) == 0u
                                       || pointerLabel.rfind("BarePtr__", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__bootList", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__bootMap", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__bootSet", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__bootString", 0u) == 0u
                                       || pointerLabel.rfind("std::lang::__bootAlgorithm", 0u) == 0u
                                       || (pointerLabel.rfind("RawStorage__", 0u) == 0u
                                           && (pointerLabel.find(".pointer") != Text::npos || pointerLabel.find(".capacity") != Text::npos
                                               || pointerLabel.find(".isInitialized") != Text::npos));
        if (state.rawIntrinsics && state.inlineHints && !state.watchdogUsed && !function.exported
            && !state.addressedFunctions.count(rawLabel) && rawPointerWrapper)
            signature += "alwaysinline ";
        else if (state.inlineHints && !state.watchdogUsed && !function.exported
                 && !state.addressedFunctions.count(textOf(function.label)) && function.end - function.labelIndex <= 12u)
            signature += "inlinehint ";
        signature += "uwtable section \"chali_code\" {\nentry:\n";
        std::size_t argumentCapacity = 1;
        for (std::size_t at = function.begin; at < function.end; ++at)
            if (state.request.quads->quads[at].op == "param") ++argumentCapacity;
        signature += "  %indirect_arguments = alloca [" + std::to_string(argumentCapacity) + " x i64]\n";
        signature += "  %native_argument_types = alloca [" + std::to_string(argumentCapacity) + " x i8]\n";
        signature += "  %raw_memory_output = alloca ptr\n  %raw_memory_input = alloca ptr\n";
        for (const auto &name : locals)
        {
            const Memory::String token(name.c_str());
            signature += "  " + pointer(token) + " = alloca " + type(token) + "\n";
            signature += "  store " + type(token) + " " + (type(token) == "double" ? "0.0" : "0") + ", ptr " + pointer(token) + "\n";
            if (type(token) == "i64") signature += "  " + tagPointer(token) + " = alloca i8\n  store i8 0, ptr " + tagPointer(token) + "\n";
        }
        for (std::size_t at = 0; at < function.parameters.size(); ++at) signature += "  store " + type(function.parameters[at]) + " %p" + std::to_string(at) + ", ptr " + pointer(function.parameters[at]) + "\n";
        signature += "  br label %q" + std::to_string(function.labelIndex) + "\n";
        for (std::size_t at = function.labelIndex; at < function.end; ++at)
        {
            body += "q" + std::to_string(at) + ":\n";
            const auto &quad = state.request.quads->quads[at];
            const auto a = quad.arg(0), b = quad.arg(1), c = quad.arg(2);
            const auto &op = quad.op;
            if (op == "label" || op == "j" || op == "jz" || op == "jnz" || op == "ret" || op == "halt"
                || op == "call" || op == "callptr" || op == "alloc" || op == "drop" || op == "free" || op == "memcpy" || op == "memset") checkedAddresses.clear();
            if (op == "label" || op == "j" || op == "jz" || op == "jnz" || op == "ret" || op == "halt"
                || op == "call" || op == "alloc" || op == "drop" || op == "free" || op == "memcpy" || op == "memset" || op.rfind("store", 0) == 0u) knownFunctions.clear();
            if (op == "label" || op == "j" || op == "jz" || op == "jnz" || op == "ret" || op == "halt"
                || op == "call" || op == "callptr" || op == "memcpy" || op == "memset" || op.rfind("store", 0) == 0u) knownLiterals.clear();
            if (op == "call" || op == "callptr" || op == "alloc" || op == "checkptr" || op == "check" || op == "memcpy" || op == "memset" || op.rfind("load", 0) == 0u || op.rfind("store", 0) == 0u || op.rfind("/", 0) == 0u || op.rfind("%", 0) == 0u || op.rfind("<<", 0) == 0u || op.rfind(">>", 0) == 0u)
            {
                const auto file = state.sourceFiles.emplace(textOf(quad.sourceFile), state.sourceFiles.size()).first;
                body += "  store ptr @" + textOf(function.llvmName) + ", ptr @chali_source_function\n";
                body += "  store ptr @source_file" + std::to_string(file->second) + ", ptr @chali_source_file\n";
                body += "  store i64 " + std::to_string(quad.sourceLine) + ", ptr @chali_source_line\n";
            }
            if (state.watchdogUsed && op != "label")
            {
                const auto cost = logicalCosts.find(quad.line);
                const auto steps = cost == logicalCosts.end() ? 1u : std::max<std::size_t>(1u, cost->second / lineOccurrences[quad.line]);
                call("chali_host_tick", "void", { { "i64", std::to_string(steps) } });
            }
            if (state.profileOwnership)
            {
                Text event;
                Value bytes{ "i64", "0" };
                if (op == "alloc")
                {
                    event = "allocation_request";
                    bytes = convert(read(a), "i64");
                }
                else if (op == "memcpy" || op == "memset")
                {
                    event = textOf(op);
                    bytes = convert(read(c), "i64");
                }
                else if (op == "drop" || op == "free")
                    event = "cleanup";
                else if (op == "call")
                    event = "call:" + textOf(a);
                else if (op == "callptr")
                    event = "indirect_call";
                else if (op == ":=" && c.rfind("__chali_move_source", 0) == 0u)
                    event = "aggregate_move";
                else if (op == ":=" && c.rfind("__chali_array_copy_source", 0) == 0u)
                    event = "array_copy";
                if (!event.empty())
                {
                    const Text site = event + "\t" + textOf(function.label) + "\t" + textOf(quad.sourceFile) + ":" + std::to_string(quad.sourceLine) + "\tquad=" + std::to_string(at);
                    const auto index = state.profileSites.emplace(site, state.profileSites.size()).first->second;
                    call("chali_backend_profile_event", "void", { { "ptr", "@backend_profile_site" + std::to_string(index) }, bytes });
                }
            }
            Memory::String base, suffix;
            splitOperation(op, base, suffix);
            bool terminated = false;
            if (op == "decl" || op == "label") {}
            else if (op == "drop" || op == "free")
                call("chali_owned_drop", "void", { convert(read(a), "i64") });
            else if (op == ":=")
            {
                const auto known = knownFunctions.find(textOf(a));
                const auto target = known == knownFunctions.end() ? Memory::String() : known->second;
                write(c, read(a));
                if (!target.empty()) knownFunctions[textOf(c)] = target;
            }
            else if (op == "addr")
            {
                const auto offset = state.offsetOf(a);
                if (offset < 0)
                {
                    state.fail("AOT：数据区里没有符号 " + a);
                    return Text();
                }
                write(c, { "i64", std::to_string(offset) });
                if (const auto *bytes = state.formatBytesOf(a)) knownLiterals[textOf(c)] = *bytes;
                const auto target = state.functionAddresses.find(static_cast<std::size_t>(offset));
                if (offset > 0 && target != state.functionAddresses.end()) knownFunctions[textOf(c)] = target->second;
            }
            else if (op == "param")
            {
                Memory::String formal = c;
                if (state.kindOf(a) == Kind::String && c.rfind("__chali_fa", 0) == 0u) formal = "#string:" + c;
                arguments.push_back({ formal, read(a) });
                /* `__std_print` 直连：捕获第一个实参（格式串字面量）的数据区字节。 */
                if (arguments.size() == 1u)
                {
                    pendingFormatValid = false;
                    const std::string *bytes = state.formatBytesOf(a);
                    const auto literal = knownLiterals.find(textOf(a));
                    if (bytes == nullptr && literal != knownLiterals.end()) bytes = &literal->second;
                    pendingFormatValid = textOf(c) == "__chali_fmt" && bytes != nullptr;
                    if (bytes != nullptr) pendingFormat = *bytes;
                }
            }
            else if (op == "call")
            {
                if (!emitCall(a, c)) return Text();
                arguments.clear();
                pendingFormatValid = false;
            }
            else if (op == "callptr")
            {
                const auto known = knownFunctions.find(textOf(a));
                const auto target = known == knownFunctions.end() ? Memory::String() : known->second;
                knownFunctions.clear();
                const auto direct = state.functionByLabel.find(textOf(target));
                if (state.directFunctionValues && !state.watchdogUsed && direct != state.functionByLabel.end()
                    && state.functions[direct->second].parameters.size() == arguments.size())
                {
                    if (!emitCall(target, c)) return Text();
                    arguments.clear();
                    pendingFormatValid = false;
                }
                else
                {
                    std::vector<std::pair<Memory::String, Value>> saved;
                    for (const auto &entry : state.temporaryUsers)
                        if (entry.second.size() == 1 && entry.second.count(textOf(function.label)) && !locals.count(entry.first))
                        {
                            const Memory::String token(entry.first.c_str());
                            saved.push_back({ token, read(token) });
                        }
                    for (std::size_t index = 0; index < arguments.size(); ++index)
                    {
                        Value value = arguments[index].second;
                        if (value.type == "double") value = instruction("i64", "bitcast double " + value.text + " to i64");
                        const auto address = instruction("ptr", "getelementptr i64, ptr %indirect_arguments, i64 " + std::to_string(index));
                        body += "  store i64 " + convert(value, "i64").text + ", ptr " + address.text + "\n";
                    }
                    Value result = call("chali_invoke", "i64", { convert(read(a), "i64"), { "i64", std::to_string(arguments.size()) }, { "ptr", "%indirect_arguments" } });
                    result.floating = instruction("i8", "load i8, ptr @chali_return_float").text;
                    for (const auto &entry : saved) write(entry.first, entry.second);
                    if (type(c) == "double") result = instruction("double", "bitcast i64 " + result.text + " to double");
                    write(c, result);
                    arguments.clear();
                    pendingFormatValid = false;
                }
            }
            else if (op == "ret" || op == "halt")
            {
                const Value value = convert(read(op == "halt" ? Memory::String() : a), resultType);
                body += "  store i8 " + (resultType == "double" ? Text("1") : (value.floating.empty() ? Text("0") : value.floating)) + ", ptr @chali_return_float\n";
                body += "  ret " + resultType + " " + value.text + "\n";
                terminated = true;
            }
            else if (op == "j" || op == "jz" || op == "jnz")
            {
                const auto target = labels.find(textOf(c));
                if (target == labels.end())
                {
                    state.fail("AOT：跳转目标不在本函数内 " + c);
                    return Text();
                }
                const Text destination = "%q" + std::to_string(target->second);
                if (state.request.options.threadsEnabled && target->second <= at) call("chali_host_poll", "void", {});
                if (op == "j")
                    body += "  br label " + destination + "\n";
                else
                {
                    Value condition = read(a);
                    const bool floating = condition.type == "double";
                    condition = instruction("i1", Text(floating ? "fcmp " : "icmp ") + (op == "jz" ? (floating ? "oeq " : "eq ") : (floating ? "une " : "ne ")) + condition.type + " " + condition.text + ", " + (floating ? "0.0" : "0"));
                    body += "  br i1 " + condition.text + ", label " + destination + ", label %q" + std::to_string(at + 1) + "\n";
                }
                terminated = true;
            }
            else if (op == "check" || op == "checkptr")
            {
                if (state.request.options.checked)
                {
                    std::vector<Value> values{ convert(read(a), "i64") };
                    values.push_back(convert(read(b), "i64"));
                    if (op == "checkptr") values.push_back(convert(read(c), "i64"));
                    call(op == "check" ? "chali_check" : "chali_host_check_pointer", "void", values);
                }
            }
            else if (op == "memcpy" || op == "memset")
            {
                const auto destination = convert(read(a), "i64");
                const auto source = convert(read(b), "i64");
                const auto bytes = convert(read(c), "i64");
                call("chali_host_memory_prepare", "void", { { "i64", op == "memcpy" ? "7" : "8" }, destination, source, bytes, { "ptr", "%raw_memory_output" }, { "ptr", "%raw_memory_input" } });
                const auto output = instruction("ptr", "load ptr, ptr %raw_memory_output");
                if (op == "memcpy")
                {
                    const auto input = instruction("ptr", "load ptr, ptr %raw_memory_input");
                    call("llvm.memcpy.p0.p0.i64", "void", { output, input, bytes, { "i1", "false" } });
                }
                else
                    call("llvm.memset.p0.i64", "void", { output, convert(source, "i8"), bytes, { "i1", "false" } });
            }
            else if (op == "alloc")
                write(c, call("chali_alloc", "i64", { convert(read(a), "i64") }));
            else if (base == "load" || base == "store")
            {
                const Width width = widthOf(suffix);
                // 与四元式 VM 及聚合封送共用物理槽：只有 u8 连续按字节保存，
                // 其他整数使用 i64、所有浮点使用 double；位宽归一属于值运算。
                const bool byteStorage = suffix == "u8";
                const Text storageType = width.float_ ? "double" : byteStorage ? "i8" :
                                                                                 "i64";
                const Value address = convert(read(a), "i64");
                const int bytes = byteStorage ? 1 : 8;
                const auto key = std::make_pair(textOf(a), bytes);
                const auto cached = checkedAddresses.find(key);
                const bool cacheable = state.cacheAddresses && !state.watchdogUsed;
                const bool reuse = cacheable && cached != checkedAddresses.end();
                const Value pointerValue = reuse ? cached->second : call("chali_host_address", "ptr", { address, { "i64", std::to_string(bytes) } });
                if (cacheable && !reuse) checkedAddresses.emplace(key, pointerValue);
                if (base == "load")
                {
                    Value result = instruction(storageType, "load " + storageType + ", ptr " + pointerValue.text + ", align 1");
                    result = convert(result, width.float_ ? "double" : "i64", width.unsigned_);
                    write(c, result);
                }
                else
                {
                    const Value value = convert(read(b), storageType, width.unsigned_);
                    body += "  store " + storageType + " " + value.text + ", ptr " + pointerValue.text + ", align 1\n";
                    checkedAddresses.clear();
                }
            }
            else if (base == "cvt")
            {
                const Width width = widthOf(suffix);
                Value source = read(a);
                if (source.type == "i64" && (b == "f32" || b == "f64")) source = instruction("double", "bitcast i64 " + source.text + " to double");
                Value result = convert(source, width.float_ ? (width.bits == 32 ? "float" : "double") : "i" + std::to_string(width.bits), width.unsigned_);
                write(c, convert(result, width.float_ ? "double" : "i64", width.unsigned_));
            }
            else if (!arithmetic(base, suffix, a, b, c))
            {
                state.fail("AOT：暂不支持的四元式操作符 " + op);
                return Text();
            }
            if (!terminated) body += "  br label %q" + std::to_string(at + 1) + "\n";
        }
        body += "q" + std::to_string(function.end) + ":\n  store i8 0, ptr @chali_return_float\n  ret " + resultType + " " + (resultType == "double" ? "0.0" : "0") + "\n}\n";
        if (!state.request.options.debug) return signature + body;
        std::map<std::size_t, DebugLine> sourceLines;
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = state.request.quads->quads[at];
            sourceLines.emplace(at, DebugLine{ textOf(quad.sourceFile), static_cast<std::size_t>(std::max<std::int32_t>(0, quad.sourceLine)) });
        }
        std::vector<DebugVariable> variables;
        for (const auto &name : locals)
        {
            const Memory::String token(name.c_str());
            const auto argument = std::find(function.parameters.begin(), function.parameters.end(), token);
            variables.push_back({ name, pointer(token), type(token) == "double", argument == function.parameters.end() ? 0u : static_cast<std::size_t>(argument - function.parameters.begin()) + 1u });
        }
        return state.debugInfo.annotate(signature + body, textOf(function.label), textOf(function.llvmName), sourceLines, variables);
    }
};

#include "chali_aot_ast.hpp"

/**
 * @brief 前端提供的 ABI 声明读取回调。
 */
Memory::Vector<ExternFunction> (*g_abiExternProvider)() = nullptr;
Memory::String (*g_abiGlueProvider)() = nullptr;
}  // namespace

namespace
{
/** @brief 前端类型化导出元数据提供方。 */
Memory::Vector<ExternFunction> (*g_exportProvider)() = nullptr;
/** @brief 前端纯 C 头文件提供方。 */
Memory::String (*g_exportHeaderProvider)() = nullptr;
}  // namespace
/**
 * @brief 注册类型化导出元数据。
 * @param provider 前端入口。
 */
void setAbiExportProvider(Memory::Vector<ExternFunction> (*provider)()) { g_exportProvider = provider; }
/**
 * @brief 取得当前导出的语言标签、C 符号与类型。
 * @return 有序元数据，无提供方时为空。
 */
Memory::Vector<ExternFunction> collectAbiExports() { return g_exportProvider ? g_exportProvider() : Memory::Vector<ExternFunction>{}; }
/**
 * @brief 注册纯 C 导出声明提供方。
 * @param provider 前端入口。
 */
void setAbiExportHeaderProvider(Memory::String (*provider)()) { g_exportHeaderProvider = provider; }
/**
 * @brief 取得纯 C 导出头文件片段。
 * @return 类型及函数声明。
 */
Memory::String collectAbiExportHeader() { return g_exportHeaderProvider ? g_exportHeaderProvider() : Memory::String(); }

/**
 * @brief 设置当前前端的 ABI 声明读取回调。
 * @param provider 返回 ABI 清单的回调；空指针清除回调。
 */
void setAbiExternProvider(Memory::Vector<ExternFunction> (*provider)())
{
    g_abiExternProvider = provider;
}

/**
 * @brief 读取当前编译单元的外部函数声明。
 * @return 前端登记的 ABI 清单；未登记时为空。
 */
Memory::Vector<ExternFunction> collectAbiExterns()
{
    return g_abiExternProvider != nullptr ? g_abiExternProvider() : Memory::Vector<ExternFunction>();
}

/**
 * @brief 登记前端生成复杂 C ABI 宿主绑定源码的入口。
 * @param provider 返回绑定源码的回调；空指针清除登记。
 */
void setAbiGlueProvider(Memory::String (*provider)())
{
    g_abiGlueProvider = provider;
}

/**
 * @brief 取得复杂 C ABI 宿主绑定源码。
 * @return 未登记或不需要封送时返回空串。
 */
Memory::String collectAbiGlue()
{
    return g_abiGlueProvider ? g_abiGlueProvider() : Memory::String();
}

/**
 * @brief 生成函数值到真实 LLVM 函数的分派，环境作为隐藏形参传入。
 * @param state 已分析的可达函数和标量类别。
 * @return 完整的 LLVM 常量及调用入口定义。
 */
Text indirectDispatcher(Translator &state)
{
    Text constants;
    Text resultTypes = "define i64 @chali_function_returns_real(i64 %function) {\nentry:\n  %target = call i64 @chali_function_target(i64 %function)\n  br label %candidate0\n";
    Text body = "define i64 @chali_invoke(i64 %function, i64 %count, ptr %arguments) {\nentry:\n  %target = call i64 @chali_function_target(i64 %function)\n  %environment = call i64 @chali_function_environment(i64 %function)\n  br label %candidate0\n";
    std::size_t candidate = 0;
    for (const auto &function : state.functions)
    {
        if (!isLive(state, function) || !state.addressedFunctions.count(textOf(function.label))) continue;
        const Text suffix = std::to_string(candidate);
        const Text label = textOf(function.label);
        Text bytes;
        for (const unsigned char byte : label)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        constants += "@indirect_name" + suffix + " = private constant [" + std::to_string(label.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
        resultTypes += "candidate" + suffix + ":\n  %matched" + suffix + " = call i64 @chali_function_match(i64 %target, ptr @indirect_name" + suffix + ")\n  %accepted" + suffix + " = icmp ne i64 %matched" + suffix + ", 0\n  br i1 %accepted" + suffix + ", label %result" + suffix + ", label %candidate" + std::to_string(candidate + 1) + "\nresult" + suffix + ":\n  ret i64 " + Text(function.returnsFloat ? "1" : "0") + "\n";
        bool environment = !function.parameters.empty() && function.parameters.back() == "__chali_environment";
        if (state.request.syntax && !function.parameters.empty())
        {
            const auto &symbols = state.astSymbols.at(textOf(function.label));
            const auto hidden = symbols.find("__chali_environment");
            environment = hidden != symbols.end() && hidden->second == function.parameters.back();
        }
        const auto arity = function.parameters.size() - (environment ? 1u : 0u);
        body += "candidate" + suffix + ":\n  %matched" + suffix + " = call i64 @chali_function_match(i64 %target, ptr @indirect_name" + suffix + ")\n";
        body += "  %name_ok" + suffix + " = icmp ne i64 %matched" + suffix + ", 0\n  %arity_ok" + suffix + " = icmp eq i64 %count, " + std::to_string(arity) + "\n  %accepted" + suffix + " = and i1 %name_ok" + suffix + ", %arity_ok" + suffix + "\n";
        body += "  br i1 %accepted" + suffix + ", label %invoke" + suffix + ", label %candidate" + std::to_string(candidate + 1) + "\ninvoke" + suffix + ":\n";
        Text arguments;
        for (std::size_t index = 0; index < function.parameters.size(); ++index)
        {
            if (index) arguments += ", ";
            const Text value = "%argument" + suffix + "_" + std::to_string(index);
            const bool floating = state.kindOf(function.parameters[index]) == Kind::Float;
            if (environment && index == arity)
                arguments += "i64 %environment";
            else
            {
                body += "  " + value + "_address = getelementptr i64, ptr %arguments, i64 " + std::to_string(index) + "\n  " + value + " = load i64, ptr " + value + "_address\n";
                if (floating) body += "  " + value + "_real = bitcast i64 " + value + " to double\n";
                arguments += (floating ? "double " + value + "_real" : "i64 " + value);
            }
        }
        body += "  %result" + suffix + " = call " + (function.returnsFloat ? "double" : "i64") + " @" + textOf(function.llvmName) + "(" + arguments + ")\n";
        if (function.returnsFloat) body += "  %bits" + suffix + " = bitcast double %result" + suffix + " to i64\n";
        body += "  ret i64 %" + Text(function.returnsFloat ? "bits" : "result") + suffix + "\n";
        ++candidate;
    }
    body += "candidate" + std::to_string(candidate) + ":\n  call void @chali_abort(i64 0)\n  unreachable\n}\n";
    resultTypes += "candidate" + std::to_string(candidate) + ":\n  call void @chali_abort(i64 0)\n  unreachable\n}\n";
    return constants + body + resultTypes;
}

/**
 * @brief 为类型化 C 包装生成索引分派，参数仍使用语言内部槽表示。
 * @param state 已解析函数及其准确种类。
 * @return 不把运行时函数值转换成机器指针的 LLVM 分派入口。
 */
Text exportDispatcher(Translator &state)
{
    if (state.request.abiExports.empty()) return {};
    Text out = "define i64 @chali_export_dispatch(i64 %index, ptr %arguments) {\nentry:\n  switch i64 %index, label %bad [";
    for (std::size_t i = 0; i < state.request.abiExports.size(); ++i)
        out += " i64 " + std::to_string(i) + ", label %case" + std::to_string(i);
    out += " ]\n";
    for (std::size_t i = 0; i < state.request.abiExports.size(); ++i)
    {
        const auto &entry = state.request.abiExports[i];
        const auto found = state.functionByLabel.find(textOf(entry.label));
        if (found == state.functionByLabel.end()) continue;
        const auto &function = state.functions[found->second];
        const auto suffix = std::to_string(i);
        out += "case" + suffix + ":\n";
        Text arguments;
        for (std::size_t j = 0; j < function.parameters.size(); ++j)
        {
            const auto name = "%v" + suffix + "_" + std::to_string(j);
            out += "  " + name + "_p = getelementptr i64, ptr %arguments, i64 " + std::to_string(j) + "\n  " + name + " = load i64, ptr " + name + "_p\n";
            const bool real = state.kindOf(function.parameters[j]) == Kind::Float;
            if (real) out += "  " + name + "_f = bitcast i64 " + name + " to double\n";
            if (j) arguments += ", ";
            arguments += real ? "double " + name + "_f" : "i64 " + name;
        }
        out += "  %result" + suffix + " = call " + Text(function.returnsFloat ? "double" : "i64") + " @" + textOf(function.llvmName) + "(" + arguments + ")\n";
        if (function.returnsFloat) out += "  %bits" + suffix + " = bitcast double %result" + suffix + " to i64\n";
        out += "  ret i64 %" + Text(function.returnsFloat ? "bits" : "result") + suffix + "\n";
    }
    return out + "bad:\n  call void @chali_abort(i64 0)\n  unreachable\n}\n";
}

/**
 * @brief 从 AST 直接生成包含运行时的 LLVM 模块，保留独立四元式调用方的兼容入口。
 * @param request 已优化的四元式、数据和输出选项。
 * @param diagnostics 接收类型、操作和运行时资产错误。
 * @param header 可选的 C ABI 头文件输出。
 * @return LLVM 文本；生成失败时为空。
 */
Memory::String generateLLVM(const Request &inputRequest, Memory::Vector<Memory::String> *diagnostics, Memory::String *header)
{
    Request request = inputRequest;
    Translator state(request, diagnostics);
    if (state.target.runtime.empty()) return {};
    if (header != nullptr) header->clear();
    if (request.quads == nullptr && request.syntax == nullptr)
    {
        state.fail("AOT：没有四元式输入");
        return {};
    }
    for (const auto &external : request.externs)
        if (external.abiConvention != "C") state.fail("AOT：外部调用约定不受支持");
    if (request.syntax) prepareAst(request, state);
    else { loadLayout(state); analyze(state); }
    for (const auto &name : request.options.exports)
        if (state.functionByLabel.count(textOf(name)) == 0u) state.fail("AOT：找不到要导出的函数 " + name);
    if (diagnostics != nullptr && !diagnostics->empty()) return {};
    for (const auto &function : state.functions)
        for (std::size_t at = function.begin; at < function.end; ++at)
            for (const auto &token : request.quads->quads[at].args)
                if (token.rfind("__temp_", 0) == 0u || token.rfind("__chali_ret_v$", 0) == 0u)
                    state.temporaryUsers[textOf(token)].insert(textOf(function.label));
    if (request.quads) for (const auto &quad : request.quads->quads)
    {
        if (quad.op != "call") continue;
        const auto found = state.externsByLabel.find(textOf(quad.arg(0)));
        if (found != state.externsByLabel.end() && (abiType(found->second->resultType) == "double" || abiType(found->second->resultType) == "float")) state.markKind(quad.arg(2), Kind::Float);
    }
    Memory::String runtime;
    const Memory::String asset = state.target.runtime;
    if (!chali_module::embedded::findGeneratorAssetByPath(asset, &runtime))
    {
        state.fail("AOT：缺少内嵌 LLVM 运行时 " + asset);
        return {};
    }
    const bool libraryLifecycle = request.options.shared && state.functionByLabel.count("__chali_library_init") != 0u;
    std::istringstream input(textOf(runtime));
    Text out = "; ChaLi LLVM AOT\n; checks: " + Text(request.options.checked ? "on" : "off") + "\nsource_filename = \"chali-aot\"\n", line;
    while (std::getline(input, line))
    {
        if (line.rfind("declare ", 0) == 0 && line.find("@chali_invoke(") != Text::npos) continue;
        if (line.rfind("; ModuleID =", 0) == 0u || line.rfind("source_filename =", 0) == 0u) continue;
        if (line.rfind("@chali_data_image =", 0) == 0u || line.rfind("@chali_data_size =", 0) == 0u || line.rfind("@chali_heap_size =", 0) == 0u) continue;
        if (state.target.windows && request.options.shared && (line.find(" @chali_init(") != Text::npos || line.find(" @chali_finish(") != Text::npos) && line.rfind("define ", 0) == 0u) line.insert(line.find("void "), "dllexport ");
        if (libraryLifecycle)
        {
            const auto init = line.find("@chali_init(");
            if (init != Text::npos) line.replace(init, 12u, "@chali_runtime_init(");
            const auto finish = line.find("@chali_finish(");
            if (finish != Text::npos) line.replace(finish, 14u, "@chali_runtime_finish(");
            const auto exported = line.find("dllexport ");
            if (exported != Text::npos) line.erase(exported, 10u);
        }
        out += line + "\n";
    }
    out += "declare i64 @chali_host_call(ptr, i64, ptr, ptr)\n";
    out += "declare i64 @chali_host_raw_storage(i64, i64, i64, i64, i64, i64)\n";
    out += "declare i64 @chali_host_list_storage(i64, i64, i64, i64, i64, i64, i64)\n";
    out += "declare i64 @chali_host_map_storage(i64, i64, i64, i64, i64, i64, i64, i64, i64)\n";
    out += "declare i64 @chali_host_string_storage(i64, i64)\n";
    out += "declare void @chali_host_memory_prepare(i64, i64, i64, i64, ptr, ptr)\n";
    out += "declare i8 @chali_host_result_float()\n";
    out += "declare double @llvm.sqrt.f64(double)\ndeclare double @llvm.floor.f64(double)\n";
    out += "declare double @llvm.ceil.f64(double)\ndeclare double @llvm.round.f64(double)\ndeclare double @llvm.trunc.f64(double)\n";
    out += "@chali_return_float = internal thread_local global i8 0\n";
    out += "@chali_source_function = internal thread_local global ptr null\n@chali_source_file = internal thread_local global ptr null\n@chali_source_line = internal thread_local global i64 0\n";
    out += "define void @chali_location(ptr %file, ptr %line, ptr %function) {\nentry:\n  %f = load ptr, ptr @chali_source_file\n  %l = load i64, ptr @chali_source_line\n  %n = load ptr, ptr @chali_source_function\n  store ptr %n, ptr %function\n  store ptr %f, ptr %file\n  store i64 %l, ptr %line\n  ret void\n}\n";
    out += "define i8 @chali_result_float() {\nentry:\n  %flag = load i8, ptr @chali_return_float\n  ret i8 %flag\n}\n";
    out += "declare void @chali_host_check_pointer(i64, i64, i64)\ndeclare void @chali_host_process_arguments(i32, ptr)\n";
    out += "declare void @chali_host_poll()\n";
    out += "declare void @chali_host_tick(i64)\n";
    out += "declare i64 @chali_host_copy_words(i64, i64, i64)\n";
    if (state.profileOwnership) out += "declare void @chali_backend_profile_event(ptr, i64)\n";
    out += "@chali_threads_enabled = constant i32 " + Text(request.options.threadsEnabled ? "1" : "0") + "\n";
    out += "@chali_step_limit = constant i64 " + std::to_string(request.options.stepLimit) + "\n";
    out += "@chali_function_count = constant i64 " + std::to_string(state.addressedFunctions.size()) + "\n";
    out += "@chali_function_addresses = constant [" + std::to_string(state.addressedFunctions.size()) + " x i64] [";
    bool firstAddress = true;
    for (const auto &label : state.addressedFunctions)
    {
        if (!firstAddress) out += ", ";
        firstAddress = false;
        out += "i64 " + std::to_string(request.dataBytes.find(Memory::String(label.c_str()) + Memory::String(1, '\0')));
    }
    out += "]\n";
    emitDataImage(state, out);
    out += "@chali_data_size = constant i64 " + std::to_string(request.dataBytes.size()) + "\n";
    out += "@chali_heap_size = constant i64 " + std::to_string(request.options.memoryBytes) + "\n";
    out += "@chali_allocator_kind = constant i32 " + Text(request.options.allocator == "pool" ? "0" : "1") + "\n";
    std::set<Text> shared;
    Text definitions;
    for (const auto &function : state.functions)
    {
        if (!isLive(state, function)) continue;
        Builder builder(state, function);
        const Text definition = request.syntax ? AstBuilder(state, function).generateAst() : builder.generate();
        if (definition.empty()) return {};
        definitions += definition;
        for (std::size_t at = function.begin; at < function.end; ++at)
        {
            const auto &quad = request.quads->quads[at];
            for (std::size_t slot = 0; slot < quad.args.size() && slot < 3; ++slot)
            {
                const auto &token = quad.args[slot];
                if (token.empty() || token == "_" || !literalIsName(token) || isTypeName(token)) continue;
                if (quad.op == "label" || quad.op == "decl" || quad.op == "j" || ((quad.op == "jz" || quad.op == "jnz") && slot > 0) || (quad.op == "call" && slot < 2) || (quad.op == "param" && slot > 0) || (quad.op == "addr" && slot < 2) || (quad.op.rfind("cvt:", 0) == 0u && slot == 1)) continue;
                if (builder.locals.count(textOf(token)) == 0u) shared.insert(textOf(token));
            }
        }
    }
    if (diagnostics != nullptr && !diagnostics->empty()) return {};
    for (const auto &entry : state.globals) shared.insert(entry.first);
    for (const auto &site : state.profileSites)
    {
        Text bytes;
        for (const unsigned char byte : site.first)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@backend_profile_site" + std::to_string(site.second) + " = private constant [" + std::to_string(site.first.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
    }
    for (const auto &file : state.sourceFiles)
    {
        Text bytes;
        for (const unsigned char byte : file.first)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@source_file" + std::to_string(file.second) + " = private constant [" + std::to_string(file.first.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
    }
    std::vector<const Function *> traced;
    for (const auto &function : state.functions)
        if (isLive(state, function)) traced.push_back(&function);
    out += "define void @chali_code_anchor() uwtable section \"chali_code\" {\nentry:\n  ret void\n}\n";
    out += "@trace_anchor_name = private constant [1 x i8] zeroinitializer\n";
    for (std::size_t at = 0; at < traced.size(); ++at)
    {
        Text bytes;
        for (const unsigned char byte : traced[at]->label)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@trace_name" + std::to_string(at) + " = private constant [" + std::to_string(traced[at]->label.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
    }
    const auto traceCount = std::to_string(traced.size() + 1);
    out += "@chali_trace_count = constant i64 " + traceCount + "\n";
    out += "@chali_trace_functions = constant [" + traceCount + " x ptr] [ptr @chali_code_anchor";
    for (const auto *function : traced) out += ", ptr @" + textOf(function->llvmName);
    out += "]\n@chali_trace_names = constant [" + traceCount + " x ptr] [ptr @trace_anchor_name";
    for (std::size_t at = 0; at < traced.size(); ++at) out += ", ptr @trace_name" + std::to_string(at);
    out += "]\n";
    for (const auto &native : state.nativeNames)
    {
        Text bytes;
        for (const unsigned char byte : native.first)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@native_name" + std::to_string(native.second) + " = private constant [" + std::to_string(native.first.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
    }
    for (std::size_t at = 0; at < state.printLiterals.size(); ++at)
    {
        const std::string &literal = state.printLiterals[at];
        Text bytes;
        for (const unsigned char byte : literal)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@chali_abi_lit" + std::to_string(at) + " = private constant [" + std::to_string(literal.size()) + " x i8] c\"" + bytes + "\"\n";
    }
    Text resetSlots = "define void @chali_reset_slots() {\nentry:\n  store i8 0, ptr @chali_return_float\n  store ptr null, ptr @chali_source_function\n  store ptr null, ptr @chali_source_file\n  store i64 0, ptr @chali_source_line\n";
    for (const auto &name : shared)
    {
        const Memory::String token(name.c_str());
        const bool floating = state.kindOf(token) == Kind::Float;
        out += "@s_" + textOf(state.use(token)) + " = internal " + Text(request.options.threadsEnabled ? "thread_local " : "") + "global " + (floating ? "double 0.0" : "i64 0") + "\n";
        if (!floating) out += "@t_" + textOf(state.use(token)) + " = internal " + Text(request.options.threadsEnabled ? "thread_local " : "") + "global i8 0\n";
        resetSlots += "  store " + Text(floating ? "double 0.0" : "i64 0") + ", ptr @s_" + textOf(state.use(token)) + "\n";
        if (!floating) resetSlots += "  store i8 0, ptr @t_" + textOf(state.use(token)) + "\n";
    }
    out += resetSlots + "  ret void\n}\n";
    std::map<Text, Text> externalDeclarations;
    for (const auto &external : request.externs)
    {
        if (state.usedExternLabels.count(textOf(external.label)) == 0u) continue;
        const Text resultType = abiType(external.resultType);
        if (resultType.empty()) continue;
        Text declaration = "declare " + resultType + " @" + llvmSymbol(external.symbol) + "(";
        for (std::size_t at = 0; at < external.parameterTypes.size(); ++at)
        {
            if (at != 0u) declaration += ", ";
            declaration += abiType(external.parameterTypes[at]);
        }
        declaration += ")\n";
        const auto existing = externalDeclarations.emplace(textOf(external.symbol), declaration);
        if (!existing.second && existing.first->second != declaration)
        {
            state.fail("AOT：同一外部符号的 ABI 声明不一致 " + external.symbol);
            return {};
        }
        if (existing.second) out += declaration;
    }
    if (request.abiGlue.empty()) out += "define i1 @chali_program_abi_bind(ptr %registry, ptr %error) {\nentry:\n  ret i1 true\n}\n";
    Memory::Vector<Memory::String> requiredNames;
    for (const auto &entry : state.nativeNames) requiredNames.push_back(entry.first.c_str());
    const auto modules = std_registry::aotModulesFor(requiredNames, diagnostics);
    out += "; AOT standard modules:";
    for (const auto &module : modules) out += " " + textOf(module);
    out += "\n";
    for (const auto &module : modules) out += "declare i32 @chali_std_bind_" + textOf(module) + "(ptr, ptr)\n";
    out += "define i1 @chali_program_std_bind(ptr %registry, ptr %error) {\nentry:\n";
    for (std::size_t i = 0; i < modules.size(); ++i)
    {
        const auto n = std::to_string(i);
        out += "  %m" + n + " = call i32 @chali_std_bind_" + textOf(modules[i]) + "(ptr %registry, ptr %error)\n  %ok" + n + " = icmp ne i32 %m" + n + ", 0\n  br i1 %ok" + n + ", label %next" + n + ", label %failed\nnext" + n + ":\n";
    }
    out += "  ret i1 true\n";
    if (!modules.empty()) out += "failed:\n  ret i1 false\n";
    out += "}\n";
    out += definitions;
    out += indirectDispatcher(state);
    out += exportDispatcher(state);
    if (libraryLifecycle)
    {
        const Text visibility = state.target.windows ? "dllexport " : "";
        out += "define " + visibility + "void @chali_init() {\nentry:\n  call void @chali_runtime_init()\n  %initialized = call i64 @f___chali_library_init()\n  ret void\n}\n";
        out += "define " + visibility + "void @chali_finish() {\nentry:\n  %finished = call i64 @f___chali_library_finish()\n  call void @chali_runtime_finish()\n  ret void\n}\n";
    }
    if (!request.options.shared) out += "define i32 @main(i32 %argc, ptr %argv) {\nentry:\n  call void @chali_init()\n  call void @chali_host_process_arguments(i32 %argc, ptr %argv)\n  %result = call i64 @f_main()\n  call void @chali_finish()\n  %code = trunc i64 %result to i32\n  ret i32 %code\n}\n";
    if (header != nullptr)
    {
        Text declarations = "/* ChaLi LLVM AOT C ABI */\n#ifndef CHALI_AOT_H\n#define CHALI_AOT_H\n#include <stdint.h>\n#include <stdbool.h>\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
        declarations += "/**\n * @brief 分配本模块的运行时内存、装入数据并执行全局初始化。\n * @note 必须先于导出函数调用，使用结束后调用 chali_finish。\n */\nvoid chali_init(void);\n";
        declarations += "/**\n * @brief 执行全局资源清理、刷新输出并释放本模块的运行时内存。\n * @note 收尾之后不能继续使用原有句柄。\n */\nvoid chali_finish(void);\n";
        for (const auto &function : state.functions)
        {
            if (!function.exported || std::any_of(request.abiExports.begin(), request.abiExports.end(), [&](const auto &entry) { return entry.label == function.label; })) continue;
            declarations += "/**\n * @brief 通过 C ABI 调用 ChaLi 函数 " + textOf(function.label) + "。\n";
            for (std::size_t at = 0; at < function.parameters.size(); ++at)
                declarations += " * @param p" + std::to_string(at) + " 按值传入 ChaLi 形参 " + textOf(function.parameters[at]) + "，句柄使用模块内字节偏移。\n";
            declarations += Text(" * @return ") + (function.returnsFloat ? "ChaLi 函数的双精度浮点返回值。" : "ChaLi 函数的六十四位返回值；无语言返回值时为零。") + "\n */\n";
            declarations += Text(function.returnsFloat ? "double " : "int64_t ") + textOf(function.llvmName) + "(";
            for (std::size_t at = 0; at < function.parameters.size(); ++at)
            {
                if (at != 0u) declarations += ", ";
                declarations += Text(state.kindOf(function.parameters[at]) == Kind::Float ? "double " : "int64_t ") + "p" + std::to_string(at);
            }
            if (function.parameters.empty()) declarations += "void";
            declarations += ");\n";
        }
        declarations += textOf(request.abiExportHeader);
        declarations += "#ifdef __cplusplus\n}\n#endif\n#endif\n";
        *header = Memory::String(declarations.c_str());
    }
    if (request.options.debug) state.debugInfo.finish(out);
    return Memory::String(out.data(), out.size());
}

namespace
{
/**
 * @brief 为宿主命令行引用单个文件路径参数。
 * @param value 文件名或工具路径。
 * @return 不参与命令替换的引用文本。
 */
Text quote(const Text &value)
{
#ifdef _WIN32
    if (value.find_first_of("\"\r\n%") != Text::npos) return Text();
    return "\"" + value + "\"";
#else
    Text result = "'";
    for (const char byte : value) result += byte == '\'' ? "'\\''" : Text(1, byte);
    return result + "'";
#endif
}

/**
 * @brief 找到当前宿主可用的 LLVM clang 驱动。
 * @param wanted 用户显式指定的 LLVM 驱动路径。
 * @return 显式路径、仓库自带工具或环境中的 clang。
 */
Text compilerPath(const Memory::String &wanted)
{
    if (!wanted.empty()) return textOf(wanted);
    if (const char *environment = std::getenv("CHALI_LLVM_CLANG"))
        if (*environment != '\0') return environment;
    std::vector<std::filesystem::path> roots{ std::filesystem::current_path() };
#ifdef _WIN32
    wchar_t buffer[32768];
    const DWORD size = GetModuleFileNameW(nullptr, buffer, 32768);
    if (size != 0u && size < 32768) roots.push_back(std::filesystem::path(buffer).parent_path());
    const char *name = "clang.exe";
#else
    std::error_code error;
    const auto executable = std::filesystem::read_symlink("/proc/self/exe", error);
    if (!error) roots.push_back(executable.parent_path());
    const char *name = "clang";
#endif
    Memory::Vector<Memory::String> toolDirectories = {"build/dependencies/llvm/toolchain/bin", "llvm/bin"};
#ifndef _WIN32
#if defined(__APPLE__)
    const Memory::String hostSystem = "macos";
#else
    const Memory::String hostSystem = "linux";
#endif
#if defined(__aarch64__) || defined(__arm64__)
    const Memory::String hostArchitecture = "arm64";
#else
    const Memory::String hostArchitecture = "x64";
#endif
    toolDirectories.insert(toolDirectories.begin(), "build/dependencies/llvm/" + hostSystem + "-" + hostArchitecture + "/toolchain/bin");
#endif
    for (const auto &root : roots)
        for (const auto &relative : toolDirectories)
        {
            const auto path = root / relative.c_str() / name;
            if (std::filesystem::is_regular_file(path)) return path.u8string();
        }
    return "clang";
}

/**
 * @brief 按二进制模式写入生成资产并检查完整写入。
 * @param path 目标路径。
 * @param contents 待写入字节。
 * @return 打开、写入和关闭全部成功时返回 true。
 */
bool writeFile(const Text &path, const Memory::String &contents)
{
    const auto parent = std::filesystem::u8path(path).parent_path();
    if (!parent.empty())
    {
        std::error_code error;
        std::filesystem::create_directories(parent, error);
        if (error) return false;
    }
    std::ofstream output(std::filesystem::u8path(path), std::ios::binary);
    output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
    output.close();
    return !output.fail();
}
}  // namespace

/**
 * @brief 将 LLVM 模块写盘，由 LLVM 优化与生成目标代码，再链接原生产物。
 * @param request 四元式、运行时数据、外部函数及输出配置。
 * @param diagnostics 接收生成、写入或 LLVM 工具失败的诊断。
 * @return 全部成功返回零；失败返回非零。
 */
int run(const Request &request, Memory::Vector<Memory::String> *diagnostics)
{
    TargetInfo target;
    Memory::String targetError;
    if (!describeTarget(request.options.target, target, &targetError))
    {
        if (diagnostics) diagnostics->push_back(targetError);
        return 1;
    }
    const bool objectOnly = request.options.emitObjectOnly;
    if (objectOnly && request.options.emitLLVMOnly)
    {
        if (diagnostics) diagnostics->push_back("AOT：LLVM 模块与对象文件输出不能同时选择");
        return 1;
    }
    if (!target.native && !objectOnly && !request.options.emitLLVMOnly && request.options.runtimeDirectory.empty())
    {
        if (diagnostics) diagnostics->push_back("AOT：跨目标链接需要 --runtime-dir 指定目标 libchali_aot_host.a，并准备目标 sysroot；可先使用 --emit-obj 或 --emit-llvm");
        return 1;
    }
    if (!target.native && !objectOnly && !request.options.emitLLVMOnly && !request.abiGlue.empty())
    {
        if (diagnostics) diagnostics->push_back("AOT：跨目标复杂 C ABI 封送需要目标 C++ 工具链；请先使用 --emit-obj 导出对象及 .abi.cpp，再在目标 SDK 中链接");
        return 1;
    }
    Memory::String header;
    const Memory::String source = generateLLVM(request, diagnostics, &header);
    if (source.empty()) return 1;
    const Text prefix = request.options.outputPrefix.empty() ? "chali_aot" : textOf(request.options.outputPrefix);
    const Text llvmPath = prefix + ".ll";
    if (!writeFile(llvmPath, source) || (!header.empty() && !writeFile(prefix + ".h", header)))
    {
        if (diagnostics != nullptr) diagnostics->push_back("AOT：无法写入 LLVM 模块或 ABI 头文件");
        return 1;
    }
    if (request.options.emitLLVMOnly) return 0;
    RuntimeAssets abiAssets;
    Request linked = request;
    if (!request.abiGlue.empty() && !objectOnly)
    {
        Memory::String object;
        if (!abiAssets.prepareAbi(request.abiGlue, &object, diagnostics)) return 1;
        linked.options.linkObjects.push_back(object);
    }
    if (objectOnly && !request.abiGlue.empty() && !writeFile(prefix + ".abi.cpp", request.abiGlue))
    {
        if (diagnostics) diagnostics->push_back("AOT：无法写入目标 ABI 封送源码");
        return 1;
    }
    const Text product = prefix + (objectOnly ? ".o" : request.options.shared ? target.windows ? ".dll" : ".so" : target.windows ? ".exe" : "");
    if (builtin::available() && request.options.compiler.empty() && (objectOnly || (target.native && request.options.sysroot.empty() && request.options.runtimeDirectory.empty())))
        return builtin::compile(linked, Memory::String(llvmPath.c_str()), source, Memory::String(product.c_str()), diagnostics);
    const Text compiler = compilerPath(request.options.compiler);
    if (quote(compiler).empty() || quote(llvmPath).empty() || quote(product).empty())
    {
        if (diagnostics != nullptr) diagnostics->push_back("AOT：工具或输出路径含不支持的命令行字符");
        return 1;
    }
    RuntimeAssets runtimeAssets;
    std::filesystem::path runtimeDirectory;
    if (!objectOnly)
    {
        if (request.options.runtimeDirectory.empty())
        {
            if (!runtimeAssets.restore(diagnostics)) return 1;
            runtimeDirectory = runtimeAssets.root;
        }
        else
        {
            runtimeDirectory = std::filesystem::u8path(textOf(request.options.runtimeDirectory));
            if (!std::filesystem::is_regular_file(runtimeDirectory / "libchali_aot_host.a"))
            {
                if (diagnostics) diagnostics->push_back("AOT：目标运行时目录缺少 libchali_aot_host.a");
                return 1;
            }
            const auto targetFile = runtimeDirectory / "target.txt";
            if (std::filesystem::is_regular_file(targetFile))
            {
                std::ifstream input(targetFile);
                std::string runtimeTarget;
                std::getline(input, runtimeTarget);
                if (!runtimeTarget.empty() && runtimeTarget.back() == '\r') runtimeTarget.pop_back();
                if (runtimeTarget != textOf(target.triple))
                {
                    if (diagnostics) diagnostics->push_back("AOT：目标运行时的 target.txt 与 --target 不一致，拒绝混用目标库");
                    return 1;
                }
            }
        }
    }
    Text command = quote(compiler) + " --target=" + quote(textOf(target.triple));
    if (!request.options.sysroot.empty())
    {
        const auto root = quote(textOf(request.options.sysroot));
        if (root.empty()) return 1;
        command += " --sysroot=" + root;
    }
    if (!request.options.cpu.empty())
    {
        const auto cpu = quote(textOf(request.options.cpu));
        if (cpu.empty()) return 1;
        command += Text(target.arm64 ? " -mcpu=" : " -march=") + cpu;
    }
    command += request.options.debug ? " -O0 -g -gdwarf-4 -Wno-override-module " : " -O3 -Wno-override-module ";
    if (objectOnly) command += "-c ";
    else
    {
        command += "-fuse-ld=lld -Wl,--gc-sections ";
        if (!request.options.debug) command += "-flto=full -Wl,--lto-O3 -s ";
        if (request.options.shared) command += "-shared ";
    }
    if (!target.windows) command += "-fPIC ";
    command += quote(llvmPath) + " -o " + quote(product);
    if (!objectOnly)
    {
        const auto runtime = quote(runtimeDirectory.generic_u8string());
        if (runtime.empty()) return 1;
        for (const auto &object : linked.options.linkObjects) command += " " + quote(textOf(object));
        command += " -L" + runtime + " -Wl,--start-group -lchali_aot_host ";
        if (request.options.runtimeDirectory.empty()) command += quote((runtimeDirectory / "libstdc++.a").generic_u8string());
        else command += "-lstdc++";
        command += " -lgcc_eh -lgcc";
        if (target.windows)
        {
            command += " -static -lwinpthread -lwinhttp -lws2_32 -lcrypt32 -lbcrypt ";
            if (request.options.runtimeDirectory.empty())
                for (const char *name : { "gcc_libmingw32.a", "gcc_libmingwex.a", "gcc_libmoldname.a" }) command += quote((runtimeDirectory / name).generic_u8string()) + " ";
        }
        else command += " -pthread -lm -ldl ";
        command += " -Wl,--end-group";
        for (const auto &library : request.options.linkLibraries)
        {
            const Text argument = quote(textOf(library));
            if (argument.empty())
            {
                if (diagnostics != nullptr) diagnostics->push_back("AOT：链接库路径含不支持的字符");
                return 1;
            }
            command += " " + argument;
        }
    }
    const Text log = prefix + ".llvm.log";
    command += " > " + quote(log) + " 2>&1";
#ifdef _WIN32
    command = "\"" + command + "\"";
#endif
#ifdef _WIN32
    const int status = _wsystem(std::filesystem::u8path(command).wstring().c_str());
#else
    const int status = std::system(command.c_str());
#endif
    if (status != 0)
    {
        if (diagnostics != nullptr)
        {
            diagnostics->push_back("AOT：LLVM 编译或链接失败；请检查 library/llvm/ 工具链或使用 --llvm 指定 clang");
            std::ifstream input(std::filesystem::u8path(log), std::ios::binary);
            Text output((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            if (output.size() > 8000) output.resize(8000);
            diagnostics->push_back(Memory::String(output.c_str()));
        }
        return 1;
    }
    return 0;
}

/**
 * @brief 直接创建原生子进程并继承标准输入、输出与错误流。
 * @param options 编译输出路径。
 * @param arguments 用户程序参数。
 * @return 子进程退出码，启动或等待失败返回一。
 */
int executeNative(const Options &options, const Memory::Vector<Memory::String> &arguments)
{
    namespace fs = std::filesystem;
    const auto product = fs::absolute(fs::u8path(std::string(options.outputPrefix.c_str())
#ifdef _WIN32
        + ".exe"
#endif
    ));
    int result = 1;
#ifdef _WIN32
    std::wstring command = tool_console::quoteArgument(product.wstring());
    for (const auto &argument : arguments) command += L" " + tool_console::quoteArgument(fs::u8path(std::string(argument.c_str())).wstring());
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    HANDLE source = GetStdHandle(STD_INPUT_HANDLE);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = source;
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    PROCESS_INFORMATION process{};
    HANDLE job = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (job) SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits));
    if (CreateProcessW(product.c_str(), command.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &startup, &process))
    {
        if (job && !AssignProcessToJobObject(job, process.hProcess)) { CloseHandle(job); job = nullptr; }
        ResumeThread(process.hThread);
        if (WaitForSingleObject(process.hProcess, INFINITE) == WAIT_OBJECT_0)
        {
            DWORD status = 1;
            if (GetExitCodeProcess(process.hProcess, &status)) result = static_cast<int>(status);
        }
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
    }
    if (job) CloseHandle(job);
#else
    std::vector<std::string> storage{ product.string() };
    for (const auto &argument : arguments) storage.emplace_back(argument.c_str());
    std::vector<char *> values;
    for (auto &argument : storage) values.push_back(argument.data());
    values.push_back(nullptr);
    const pid_t child = fork();
    if (child == 0)
    {
        execv(product.c_str(), values.data());
        _exit(127);
    }
    if (child > 0)
    {
        int status = 0;
        pid_t waited;
        do { waited = waitpid(child, &status, 0); } while (waited < 0 && errno == EINTR);
        if (waited == child) result = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    }
#endif
    return result;
}
}  // namespace chali_aot
