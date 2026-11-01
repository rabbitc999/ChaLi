/* ============================================================
 * 入口：chali <源文件.chl> [--out 前缀] [--debug]
 *                           [-I 目录] [-D 符号[=值]] [--raw] [--deps]
 *
 *   ① 读源文件（不给文件也不给 `--stdin` 时显示帮助）；
 *   ② **前处理**：`%if` / `%define` 等指令（source/preprocess，
 *      语言无关的那一份），默认用空行占位，所以行号与源文件一一对应；
 *   ③ 解析：文法动作在归约的那一刻就把自己的 C 语法树建好；
 *   ④ M_Compile：降级到 ast → 出 .quad / .data → stackvm 组装执行。
 *
 * %main 替换内建 demo main，外壳（stdio / string / parser.hpp）仍是模板给的，
 * 正文前面自动补了 `using namespace chali;`。
 * ============================================================ */
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

/* 前处理器（仓库里的通用库，只依赖 C++17 标准库）。 */
/* 后端统计：`--stats` 里要报指令选择覆盖率与帧槽复用前后的数字。 */
#include "register_allocate.hpp"
#include "instruction_select.hpp"
#include <algorithm>
#include "preprocess.hpp"
#include "chali_diagnostics.hpp"
#include "chali_warnings.hpp"
#include "macro/source.hpp"
#include "macro/fragments.hpp"
#include "editor/compiler_analysis.hpp"
#include "chali_interpolation.hpp"
#include "chali_module.hpp"
#include "chali_precompiled.hpp"
#include "chali_incremental.hpp"
#include "chali_analysis.hpp"
#include "chali_lambda.hpp"
#include "chali_generic.hpp"
#include "chali_project.hpp"
#include "chali_cli.hpp"
#include "chali_aot.hpp"
#include "ast/ast.hpp"
#include "chali_aot_llvm.hpp"
#include "abi/chali_abi_registry.hpp"
#include "abi/chali_standard_internal.h"
#include "chali_report.hpp"
#include "abi/chali_standard.h"

namespace
{

/**
 * @brief 按显示设置保留完整宏展开栈或仅显示最近的展开帧。
 * @param message 包含字段、实参和展开帧的完整诊断。
 * @param full 是否保留全部展开帧。
 * @return 用于文本或 JSON 输出的诊断，不改变语义检查结果。
 */
Memory::String MacroDiagnosticDisplay(const Memory::String &message, bool full)
{
    if (full) return message;
    Memory::String result;
    bool seen = false, omitted = false;
    for (std::size_t begin = 0; begin <= message.size();)
    {
        const auto end = message.find('\n', begin);
        const auto line = message.substr(begin, end == Memory::String::npos ? Memory::String::npos : end - begin);
        const bool frame = line.rfind("  展开宏 $", 0) == 0 || line.rfind("  expanding macro $", 0) == 0
                           || line.rfind("  派生 #derive(", 0) == 0;
        if (frame && seen) omitted = true;
        else
        {
            if (!result.empty()) result += '\n';
            result += line;
        }
        if (frame) seen = true;
        if (end == Memory::String::npos) break;
        begin = end + 1;
    }
    if (omitted) result += "\n  --macro-backtrace full";
    return result;
}

/**
 * @brief 注册与 AOT 共享的标准库和演示宿主接口。
 * @param registry 接收函数绑定的宿主注册表。
 * @return 无。
 */
void InstallDemoGlue(ChaliAbiRegistry *registry)
{
    char message[1024];
    ChaliAbiError error{message, sizeof(message), 0, ChaliAbiOk};
    const ChaliAbiModule demo[] = {abi_demo_records};
    if (chali_abi_bind_standard(registry, &error) != ChaliAbiOk ||
        chali_abi_register_tables(registry, demo, 1, &error) != ChaliAbiOk)
        std::fprintf(stderr, "chali: 注册宿主胶水失败：%s\n", message);
}

/**
 * @brief 选项要一个值：`-I dir` / `--include-dir dir` 两种写法都行。
 * @param index 待访问的元素索引。
 * @param argc 命令行参数数量。
 * @param what 测试条件的说明文本。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool NeedValue(int index, int argc, const char *what)
{
    if (index + 1 < argc) return true;
    std::fprintf(stderr, CHALI_TEXT("选项 %s 后面要跟一个值\n"), what);
    return false;
}

/**
 * @brief `-h` / `--help`：把全部命令行选项列一遍（不回显源文件，直接退出 0）。
 * @param out 接收处理结果的输出对象。
 */
void PrintUsage(std::FILE *out)
{
    for (const char *line : {
             "--------------------------------------------------------\n",
             "ChaLi 编译器\n",
             "--------------------------------------------------------\n",
             "C -- Copy（深拷贝值语义）：赋值就是彻底的复制，绝不共享内存。大对象必须显式用引用传。\n",
             "H -- Hygienic（卫生宏）：在编译期做安全的代码替换，绝不意外捕获或污染外部变量。\n",
             "A -- Any（万能盒子 / 类型擦除）：没有泛型约束时，可以装下任何类型的数据。\n",
             "L -- Lifetime（生命周期 / 借用检查器）：引用永远不能活得比它的数据源更久，否则编译直接报错（CHL1204）。\n",
             "I -- Interface（接口 / 运行期多态）：定义一组共同的“契约”，运行期根据对象的实际类型进行分派。\n",
             "--------------------------------------------------------\n",
             "用法：chali 文件.chl [程序参数...]\n",
             "      chali 场景 [文件.chl] [配置词...] [-- 程序参数...]\n",
             "\n",
             "统一工具链：AST 直接生成 LLVM，支持原生运行与交叉目标代码生成。\n",
             "场景指令（运行方式也可用 --mode 指定）：\n",
             "  run|interpret  checked AOT 编译执行（缺省）；直接传入文件时，其后全为程序参数\n",
             "  check FILE [json]    仅检查，可输出 JSON 诊断\n",
             "  help                 显示帮助\n",
             "  lib            库包工具：template、pack、inspect、lock\n"
             "  link           链接原生对象与归档\n"
             "  ar             创建确定性静态归档\n"
             "  asm            将目标 .s 汇编为对象文件，不执行链接\n"
             "  aot            unchecked LLVM 产物（--target、--emit-llvm、--emit-obj、--emit-asm、--shared、--export）\n",
             "  new DIR [name NAME]  新建 ChaLi 应用工程（目标目录必须不存在）\n",
             "  gen            生成独立编译器工程（转发 chali-generator compiler）\n",
             "  clear out DIR  清理登记过的中间产物（不重新编译）\n",
             "  lang zh|en|auto     选择显示语言；默认自动检测系统语言\n",
             "常用配置词：debug、out 路径、reflect；new 使用 name 工程名。\n",
             "开发场景：test tests backend interpret、fmt check src、deps lock project . offline。\n",
             "  man [目标]            查询内嵌标准库手册、参数、边界与示例\n",
             "  test|fmt|lsp|deps      测试、格式化、语言服务与依赖锁定（原生内建）\n",
             "直接文件调用始终运行；文件后的所有内容传给程序。显式场景的高级选项：\n",
             "不给源文件时显示本帮助；要从标准输入读源码，显式给 --stdin。选项：\n",
             "  -h, --help             显示本帮助并退出\n",
             "  -D, --define 符号[=值] 定义预处理符号（也可写 -D符号=值）\n",
             "  -I, --include-dir 目录 追加模块搜索目录（也可写 -I目录）\n",
             "  --check                仅检查，不执行、不生成产物\n",
             "  --debug                保留原生符号、源码行和变量信息，使用 O0\n",
             "  --deps                 只列出 import 模块与工程依赖文件（构建脚本用）\n",
             "  --library-lock 文件    按精确版本及摘要检查预编译库\n",
             "  --diagnostics=json     检查并输出稳定 JSON 诊断\n"
             "  --warn=规则 --allow=规则 --deny=规则  配置警告（all／naming／稳定代码）\n"
             "  --warnings-as-errors  把启用的警告提升为错误\n",
             "  --macro-backtrace short|full  宏诊断展开栈显示；缺省 full\n",
             "  --target 三元组        windows-x64、linux-x64、linux-arm64 或规范 LLVM 三元组\n",
             "  --asm 文件.s          汇编后加入最终链接，可重复指定\n"
             "  --asm-include 目录     汇编 .include 的搜索目录，可重复指定\n"
             "  --emit-asm             生成目标机器汇编 .s，不链接；可配合 --target\n"
             "  --emit-obj             生成目标 .o，不链接；可配合 --target 交叉生成\n",
             "  --sysroot 目录         目标 SDK 的系统根目录\n",
             "  --runtime-dir 目录     目标 libchali_aot_host.a 所在目录\n",
             "  --cpu 名称             目标 CPU；也可在 chali.toml 的 [aot] 表配置\n",
             "  --allocator malloc|pool  选择系统 malloc 或内存池；缺省 malloc\n",
             "  --hard-realtime        禁止整个编译单元使用反射\n",
             "  --import-dir 目录      追加 import 搜索目录\n",
             "  --include-path 目录    追加模块搜索目录\n",
             "  --incremental          跨次复用模块预处理与完整编译产物\n",
             "  --memory-size 字节     运行时堆容量（缺省 512MiB，最小 64）\n",
             "  --no-debug             （兼容别名）显式不带调试信息；现在也是缺省\n",
             "  --no-reflection        不写反射元数据（反射不可裁剪，编译期处理照常）\n",
             "  --no-stdlib-cache      关闭内嵌标准库预编译（每次完整解析，不写缓存）\n",
             "  --no-threads           关闭隔离的宿主线程执行（缺省启用）\n",
             "  --out 前缀             生成产物的文件名前缀（缺省 .chali/chali_out）\n",
             "  --keep-artifacts       保留 .chali 下的中间产物（缺省运行或导出完成后清理）\n",
             "  --raw                  跳过前处理：源文件原样喂给解析器\n",
             "  --reflection           写入反射元数据（缺省已写入；反射不可裁剪）\n",
             "  --stdin                从标准输入读源码（不给文件时缺省改为显示本帮助）\n",
             "  --stats                打印运行时、增量校验与后端两层（指令选择 / 帧槽）统计\n",
             "  --step-limit N         本次执行的指令条数上限（缺省不设上限；0 同缺省）\n",
             "  --                     后面全部作为程序的命令行参数\n",
         })
        std::fputs(CHALI_TEXT(line), out);
}

}  // namespace

/**
 * @brief 获取内嵌开发工具资产。
 * @param name 仓库相对路径。
 * @return UTF-8 文本；进程内持续有效。
 */
const char *chali_development_tool(const char *name)
{
    static std::map<std::string, Memory::String> sources;
    auto &source = sources[name];
    if (source.empty()) chali_module::embedded::findGeneratorAssetByPath(name, &source);
    return source.c_str();
}

/**
 * @brief 解析前端命令行参数并执行编译或运行任务。
 * @param argc 命令行参数数量。
 * @param argv 命令行参数指针数组。
 * @retval 进程退出状态，0 表示成功，非零表示失败。
 */
/**
 * @brief 删除 .chali 下的中间产物，保留显式导出物。
 * @param prefix 中间产物前缀（缺省 .chali/chali_out）。
 * @param withExecutable 是否连运行用的临时可执行文件一起删除。
 * @param withHeader 是否连 ABI 导出头一起删除；带 `--export` 时头文件是交付物，必须留下。
 * @return 无。
 */
static void RemoveIntermediateArtifacts(const Memory::String &prefix, bool withExecutable, bool withHeader)
{
    for (const char *suffix : { ".quad", ".data.bin", ".layout", ".ll", ".llvm.log", ".abi.cpp" })
    {
        std::error_code error;
        std::filesystem::remove(Memory::u8path(prefix + suffix), error);
    }
    if (withHeader)
    {
        std::error_code error;
        std::filesystem::remove(Memory::u8path(prefix + ".h"), error);
    }
    if (!withExecutable) return;
    std::error_code error;
    std::filesystem::remove(Memory::u8path(prefix + ".exe"), error);
}

/**
 * @brief 编译或执行源码，并将前端分配失败转换为有界的非零退出诊断。
 * @param argc 命令行实参数量。
 * @param argv 命令行实参；包含程序路径。
 * @return 成功返回零，参数、编译或执行失败返回非零状态。
 */
int chali_main_entry(int argc, char **argv)
try
{
#ifndef CHALI_FRONT_END_NO_MAIN
    auto originalArguments = chali_i18n::processArguments(argc, argv);
    std::vector<char *> originalValues;
    for (auto &argument : originalArguments) originalValues.push_back(argument.data());
    auto normalized = chali_cli::normalize(static_cast<int>(originalArguments.size()), originalValues.data());
    std::string localeError;
    if (!chali_i18n::configure(normalized, localeError))
    {
        std::fprintf(stderr, "chali: %s\n", localeError.c_str());
        return 2;
    }
    std::vector<char *> normalizedArgv;
    for (auto &argument : normalized) normalizedArgv.push_back(argument.data());
    normalizedArgv.push_back(nullptr);
    argc = static_cast<int>(normalized.size());
    argv = normalizedArgv.data();
#endif
    if (argc == 2 && std::strcmp(argv[1], "--build-info") == 0)
    {
        std::printf("{\"version\":\"%s\",\"llvm\":\"%s\",\"execution\":\"aot\",\"vm\":false,\"svmb\":false}\n",
                    chali_package::compilerVersion, chali_aot::builtin::available() ? "static" : "external");
        return 0;
    }
    if (argc > 1 && std::strcmp(argv[1], "new") == 0) return chali_project::runCli(argc, argv);
    stackvm::NativeRegistry nativeRegistry;
    ChaliAbiRegistry glue{{}, &nativeRegistry};
    InstallDemoGlue(&glue);
    M_SetNativeRegistry(&nativeRegistry);
    const char *path = nullptr;
    /* 不给源文件时不再默默挂到标准输入上：默认把帮助打出来就退出，真要从管道喂源码得显式写 `--stdin`。 */
    bool readStdin = false;
    /* 默认中间产物落在隐藏的 .chali/ 下，不和源码混在一起（`--out` 显式给路径时按用户给的来）。 */
    Memory::String prefix = ".chali/chali_out";
    bool outGiven = false;      /* 用户是否显式给了 --out。 */
    bool keepArtifacts = false; /* --keep-artifacts：保留中间产物，不在完成后清理。 */
    int run = 1;
    bool checkOnly = false, jsonDiagnostics = false;
    chali_warnings::Options warningOptions;
    Memory::Vector<std::pair<Memory::String, Memory::String>> warningOverrides;
    bool macroBacktraceFull = true;
    bool hardRealtime = false;
    /* 注解与反射默认开启：宿主（本 exe）没编入支持时静默降级为关闭，不打扰用户；
     * 只有显式写了 `--reflection` 才按"要求必须有支持"处理（不支持就报错）。 */
    bool reflectionEnabled = true;
    bool reflectionExplicit = false; /* 用户是否显式给了 --reflection / --no-reflection。 */

    bool aot = false;
    chali_aot::Options aotOptions;
    Memory::String libraryLock, libraryModule, libraryVersion;
    bool raw = false;            /* --raw：不跑前处理 */
    bool depsOnly = false;       /* --deps：只列出依赖文件 */
    bool threadsEnabled = true;  /* --no-threads：关闭隔离的宿主线程 */
    bool debugEnabled = false; /* --debug：保留原生符号、源码行与变量信息，O0 构建 */
    bool dapEnabled = false;   /* --dap：作为 VS Code 的调试适配器跑（stdio 上收发 DAP） */
    bool stripDebug = true;    /* 缺省 --strip-all；--debug 才保留调试信息 */
    std::size_t memoryBytes = Memory::defaultRuntimeCapacity;
    std::size_t stepLimit = 0u; /* --step-limit：0 表示不设上限（缺省） */
    bool runtimeStats = false;
    bool incremental = false; /* --incremental：跨次复用模块预处理及完整编译产物 */
    /* 标准库预编译单元：跨次复用标准库段的解析产物；`--no-stdlib-cache` 关掉。 */
    bool standardUnit = std::getenv("CHALI_NO_STDLIB_CACHE") == nullptr;
    Memory::String buildStandardUnit, buildStandardModule, buildStandardModuleOutput, buildStandardTemplates, buildStandardTemplatesOutput;
    bool sourceStdin = false;  ///< 编辑器用真实文件身份读取标准输入中的未保存内容。
    Memory::String cacheDirectory, overlayPath, analysisDependencies, analysisTypes;
    Memory::Vector<Memory::String> includeDirs;
    Memory::Vector<Memory::String> importDirs;
    Memory::Vector<Memory::String> moduleRuntimeDirs;
    {
        std::error_code ec;
        const std::filesystem::path executable =
            std::filesystem::weakly_canonical(std::filesystem::u8path(argv[0]), ec);
        if (!ec && executable.has_parent_path())
            moduleRuntimeDirs.push_back(executable.parent_path().u8string());
    }
    Memory::Vector<Memory::String> defineItems;
    Memory::Vector<Memory::String> programArguments;
    /* 统一入口支持 VM 执行、中间产物导出及显式 AOT 编译。 */
    int firstArgument = 1;
    if (argc > 1)
    {
        const Memory::String first = argv[1];
        if (first == "run" || first == "interpret")
        {
            firstArgument = 2;

        }
        else if (first == "aot")
        {
            aot = true;
            aotOptions.checked = false;

            run = 0;
            firstArgument = 2;
        }
        else if (first == "emit")
        {
            std::fprintf(stderr, CHALI_TEXT("chali: --emit 已删除，请直接使用 run 或 aot。\n"));
            return 2;
        }
        else if (first == "svmb" || first == "smvb")
        {
            std::fputs("chali: .svmb 导出已删除，请使用 aot 导出原生产物。\n", stderr);
            return 2;
        }
    }
    for (int index = firstArgument; index < argc; ++index)
    {
        const Memory::String argument = argv[index];
        if (argument == "-h" || argument == "--help")
        {
            PrintUsage(stdout);
            return 0;
        }
        if (argument == "--library-module" || argument == "--library-version")
        {
            if (++index >= argc)
            {
                std::fputs("chali lib: 模块或包版本缺少值\n", stderr);
                return 2;
            }
            if (argument == "--library-module")
            {
                libraryModule = argv[index];
                aotOptions.shared = true;
            }
            else
                libraryVersion = argv[index];
            continue;
        }
        if (argument == "--library-lock")
        {
            if (++index >= argc)
            {
                std::fputs("chali: --library-lock 缺少路径\n", stderr);
                return 2;
            }
            libraryLock = argv[index];
            continue;
        }
        if ((aot || run) && (argument == "--checked" || argument == "--unchecked" || argument == "--shared" || argument == "--emit-llvm" || argument == "--emit-obj" || argument == "--emit-asm"))
        {
            if (argument == "--checked") aotOptions.checked = true;
            if (argument == "--unchecked") aotOptions.checked = false;
            if (argument == "--shared") aotOptions.shared = true;
            if (argument == "--emit-llvm") aotOptions.emitLLVMOnly = true;
            if (argument == "--emit-asm") aotOptions.emitAssemblyOnly = true;
            if (argument == "--emit-obj") aotOptions.emitObjectOnly = true;
            continue;
        }
        if ((aot || run) && (argument == "--llvm" || argument == "--export" || argument == "--link" || argument == "--asm" || argument == "--asm-include" || argument == "--target" || argument == "--cpu" || argument == "--sysroot" || argument == "--runtime-dir" || argument == "--allocator"))
        {
            if (index + 1 >= argc)
            {
                std::fprintf(stderr, "chali: %s 缺少取值\n", argument.c_str());
                return 2;
            }
            const Memory::String value = argv[++index];
            if (argument == "--llvm") aotOptions.compiler = value;
            if (argument == "--export") aotOptions.exports.push_back(value);
            if (argument == "--asm") aotOptions.assemblySources.push_back(value);
            if (argument == "--asm-include") aotOptions.assemblyIncludes.push_back(value);
            if (argument == "--link") aotOptions.linkLibraries.push_back(value);
            if (argument == "--target") aotOptions.target = value;
            if (argument == "--allocator") aotOptions.allocator = value;
            if (argument == "--cpu") aotOptions.cpu = value;
            if (argument == "--sysroot") aotOptions.sysroot = value;
            if (argument == "--runtime-dir") aotOptions.runtimeDirectory = value;
            continue;
        }
        /* `--mode`：与统一入口一致的运行方式；interpret / run / aot 折到 run，svmb 已删除。 */
        if (argument == "--mode" || argument == "--run-mode" || argument.rfind("--mode=", 0) == 0 || argument.rfind("--run-mode=", 0) == 0)
        {
            const auto separator = argument.find('=');
            if (separator == Memory::String::npos && index + 1 >= argc)
            {
                std::fprintf(stderr, CHALI_TEXT("chali: %s 缺少取值\n"), argument.c_str());
                return 2;
            }
            const Memory::String mode = separator == Memory::String::npos ? Memory::String(argv[++index]) : argument.substr(separator + 1);

            if (mode == "interpret") { /* 缺省 */ }
            else if (mode == "aot")
            {
                aot = true;
                aotOptions.checked = false;
                run = 0;
            }
            else if (mode == "emit")
            {
                std::fprintf(stderr, CHALI_TEXT("chali: --emit 已删除，请直接使用 run 或 aot。\n"));
                return 2;
            }
            else
            {
                std::fprintf(stderr, CHALI_TEXT("chali: 不认识的运行方式：%s\n"), mode.c_str());
                return 2;
            }
            continue;
        }
        if (argument == "--")
        {
            for (++index; index < argc; ++index) programArguments.emplace_back(argv[index]);
            break;
        }
        if (argument == "--out")
        {
            if (!NeedValue(index, argc, "--out")) return 2;
            prefix = argv[++index];
            outGiven = true;
            continue;
        }
        if (argument == "--keep-artifacts")
        {
            keepArtifacts = true;
            continue;
        }
        if (argument == "--memory-size")
        {
            if (index + 1 >= argc)
            {
                std::fprintf(stderr, CHALI_TEXT("--memory-size 缺少容量\n"));
                return 2;
            }
            const Memory::String value = argv[++index];
            if (value.empty() || value.find_first_not_of("0123456789") != Memory::String::npos) return 2;
            try
            {
                memoryBytes = static_cast<std::size_t>(std::stoull(value));
            } catch (...) { return 2; }
            if (memoryBytes < 64) return 2;
            continue;
        }
        if (argument == "--hard-realtime")
        {
            hardRealtime = true;
            continue;
        }
        if (argument == "--dap")
        {
            dapEnabled = true;
            continue;
        }
        if (argument == "--reflection")
        {
            reflectionEnabled = true;
            reflectionExplicit = true;
            continue;
        }
        if (argument == "--no-reflection")
        {
            reflectionEnabled = false;
            reflectionExplicit = true;
            continue;
        }
        if (argument == "--abi-cache" || argument == "--abi-bridge" || argument == "--abi-write-bridge" ||
            argument == "--abi-no-cache" || argument == "--abi-prepare")
        {
            std::fprintf(stderr, "chali: 旧解释执行 ABI 选项 %s 已删除，请直接使用 run 或 aot。\n", argument.c_str());
            return 2;
        }
        if (argument == "--check")
        {
            checkOnly = true;
            run = 0;
            continue;
        }
        if (argument == "--diagnostics=json")
        {
            checkOnly = true;
            jsonDiagnostics = true;
            run = 0;
            continue;
        }
        if (argument == "--warnings-as-errors")
        {
            warningOptions.errors = true;
            continue;
        }
        if (argument == "--warning-profile=editor")
        {
            warningOptions.set("naming", "warn");
            continue;
        }
        if (argument.compare(0, 7, "--warn=") == 0 || argument.compare(0, 8, "--allow=") == 0 || argument.compare(0, 7, "--deny=") == 0)
        {
            const auto split = argument.find('=');
            const auto rule = argument.substr(split + 1);
            const auto level = argument.compare(0, 7, "--warn=") == 0 ? "warn" : argument.compare(0, 8, "--allow=") == 0 ? "allow" :
                                                                                                                           "error";
            if (!warningOptions.set(rule, level))
            {
                std::fprintf(stderr, "chali: 未知警告规则：%s\n", rule.c_str());
                return 2;
            }
            warningOverrides.push_back({ rule, level });
            continue;
        }
        if (argument == "--macro-backtrace")
        {
            if (index + 1 >= argc || (Memory::String(argv[index + 1]) != "short" && Memory::String(argv[index + 1]) != "full"))
            {
                std::fprintf(stderr, "chali: --macro-backtrace short|full\n");
                return 1;
            }
            macroBacktraceFull = Memory::String(argv[++index]) == "full";
            continue;
        }
        if (argument == "--source-stdin")
        {
            sourceStdin = true;
            continue;
        }
        if (argument == "--stdin")
        {
            readStdin = true;
            continue;
        }
        if (argument == "--no-stdlib-cache")
        {
            standardUnit = false;
            continue;
        }
        if (argument == "--stdlib-cache-dir")
        {
            std::fputs(CHALI_TEXT("chali: --stdlib-cache-dir 已删除；标准库预编译资产由编译器内嵌。\n"), stderr);
            return 2;
        }
        if (argument == "--build-stdlib-templates")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            buildStandardTemplates = argv[++index];
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            buildStandardTemplatesOutput = argv[++index];
            continue;
        }
        if (argument == "--build-stdlib-module")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            buildStandardModule = argv[++index];
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            buildStandardModuleOutput = argv[++index];
            continue;
        }
        if (argument == "--build-stdlib-unit")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            buildStandardUnit = argv[++index];
            continue;
        }
        if (argument == "--cache-dir" || argument == "--source-overlays" || argument == "--analysis-deps" || argument == "--analysis-types")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            const Memory::String value = argv[++index];
            if (argument == "--cache-dir")
                cacheDirectory = value;
            else if (argument == "--source-overlays")
                overlayPath = value;
            else if (argument == "--analysis-deps")
                analysisDependencies = value;
            else
                analysisTypes = value;
            continue;
        }
        if (argument == "--stats")
        {
            runtimeStats = true;
            continue;
        }
        if (argument == "--step-limit") /* 0 表示不设上限（缺省），正整数用于防死循环 */
        {
            if (!NeedValue(index, argc, "--step-limit")) return 2;
            const Memory::String value = argv[++index];
            if (value.empty() || value.find_first_not_of("0123456789") != Memory::String::npos)
            {
                std::fprintf(stderr, CHALI_TEXT("--step-limit 需要非负整数（0 表示不设上限）\n"));
                return 2;
            }
            try
            {
                stepLimit = static_cast<std::size_t>(std::stoull(value));
            } catch (...)
            {
                std::fprintf(stderr, CHALI_TEXT("--step-limit 需要非负整数（0 表示不设上限）\n"));
                return 2;
            }
            continue;
        }
        if (argument == "--emit") /* 已删除 */
        {
            std::fprintf(stderr, CHALI_TEXT("chali: --emit 已删除，请直接使用 run 或 aot。\n"));
            return 2;
        }
        if (argument == "--no-threads")
        {
            threadsEnabled = false;
            continue;
        }
        if (argument == "--debug") /* 带调试信息：O0 构建并保留符号、源码行与变量 */
        {
            debugEnabled = true;
            stripDebug = false;
            continue;
        }
        if (argument == "--incremental")
        {
            incremental = true;
            continue;
        }
        if (argument == "--svmb")
        {
            std::fputs("chali: --svmb 已删除，请使用 aot。\n", stderr);
            return 2;
        }
        if (argument == "--no-debug") /* 兼容别名：显式不带调试节（现在也是缺省） */
        {
            stripDebug = true;
            debugEnabled = false;
            continue;
        }
        if (argument == "--raw") /* 跳过前处理：把源文件原样喂给解析器 */
        {
            raw = true;
            continue;
        }
        if (argument == "--deps") /* 只列出 import 模块与工程依赖文件（构建脚本用） */
        {
            depsOnly = true;
            continue;
        }
        if (argument == "-I" || argument == "--include-dir")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            includeDirs.push_back(argv[++index]);
            importDirs.push_back(includeDirs.back());
            continue;
        }
        if (argument.compare(0u, 2u, "-I") == 0 && argument.size() > 2u) /* -I目录 */
        {
            includeDirs.push_back(argument.substr(2u));
            importDirs.push_back(includeDirs.back());
            continue;
        }
        if (argument == "--include-path" || argument == "--import-dir")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            auto &directories = argument == "--include-path" ? includeDirs : importDirs;
            directories.push_back(argv[++index]);
            if (argument == "--include-path") importDirs.push_back(directories.back());
            continue;
        }
        if (argument == "-D" || argument == "--define")
        {
            if (!NeedValue(index, argc, argument.c_str())) return 2;
            defineItems.push_back(argv[++index]);
            continue;
        }
        if (argument.compare(0u, 2u, "-D") == 0 && argument.size() > 2u) /* -D符号=值 */
        {
            defineItems.push_back(argument.substr(2u));
            continue;
        }
        if (!argument.empty() && argument[0] == '-')
        {
            std::fprintf(stderr, CHALI_TEXT("不认识的选项：%s\n"), argument.c_str());
            return 2;
        }
        if (path != nullptr)
        {
            std::fprintf(stderr, CHALI_TEXT("一次只能编译一个源文件\n"));
            return 2;
        }
        path = argv[index];
    }

    /* 既没给源文件也没要 `--stdin`：显示帮助并退出 0。以前这里会一直等标准输入，
     * 管道没接上时看起来像卡死；现在把「要读管道」变成显式动作。 */
    if (path == nullptr && !readStdin && !aot)
    {
        PrintUsage(stdout);
        return 0;
    }

    /* 缺省中间产物目录：Windows 上设为隐藏，避免污染工程目录；--out 显式指定时不动用户的目录。 */
    if (!outGiven && prefix.rfind(".chali", 0) == 0)
    {
        std::error_code artifactDirectoryError;
        std::filesystem::create_directories(".chali", artifactDirectoryError);
#ifdef _WIN32
        SetFileAttributesA(".chali", FILE_ATTRIBUTE_HIDDEN);
#endif
    }

    /* 硬实时禁止反射：默认开启的那份自动让位，只有用户显式点名要反射才算冲突。 */
    if (hardRealtime && reflectionEnabled)
    {
        if (reflectionExplicit)
        {
            const char *message = CHALI_TEXT("硬实时编译禁止启用反射");
            if (jsonDiagnostics)
                chali_diagnostics::report("CHL1102", message, path ? path : "<命令行>");
            else
                std::fprintf(stderr, "chali: %s\n", message);
            return 2;
        }
        reflectionEnabled = false;
    }
    if (!M_SetReflectionEnabled(reflectionEnabled) && reflectionExplicit)
    {
        /* 默认开启只是"能用就用"：宿主没编入支持时静默关掉，不能让每个程序都失败。
         * 显式 `--reflection` 才按硬要求处理，报出缺哪个编译宏。 */
        const char *message = CHALI_TEXT("--reflection 需要 CHALI_WITH_REFLECTION=1 且运行时 STACKVM_WITH_REFLECTION=1");
        if (jsonDiagnostics)
            chali_diagnostics::report("CHL1102", message, path ? path : "<命令行>");
        else
            std::fprintf(stderr, "chali: %s\n", message);
        return 2;
    }
    if (!aot && incremental && (path == nullptr || raw))
    {
        std::fprintf(stderr, CHALI_TEXT("chali: --incremental 需要源文件，且不能与 --raw 合用\n"));
        return 2;
    }
    if (run) { aot = true; aotOptions.checked = true; }
    aotOptions.debug = debugEnabled;
    if (sourceStdin && (!path || !checkOnly))
    {
        std::fputs("chali: --source-stdin 需要 --check 和源文件路径\n", stderr);
        return 2;
    }
    Memory::Map<Memory::String, Memory::String> sourceOverlays;
    try
    {
        sourceOverlays = chali_analysis::overlays(overlayPath);
    } catch (const std::exception &error)
    {
        if (jsonDiagnostics)
            chali_diagnostics::report("CHL1102", error.what(), "<命令行>");
        else
            std::fprintf(stderr, "chali: %s\n", error.what());
        return 2;
    }
    chali_package::Project project;
    Memory::String packageError;
    if (!raw && !chali_package::discover(path ? path : "", project, packageError))
    {
        if (jsonDiagnostics)
            chali_diagnostics::report("CHL1101", packageError, project.file);
        else
            std::fprintf(stderr, "chali: %s\n", packageError.c_str());
        return 1;
    }
    for (const auto *group : { "all", "naming" })
        if (project.warnings.count(group)) warningOptions.set(group, project.warnings.at(group));
    for (const auto &entry : project.warnings)
    {
        if (entry.first == "all" || entry.first == "naming") continue;
        if (!warningOptions.set(entry.first, entry.second))
        {
            std::fprintf(stderr, "chali: 未知警告规则：%s\n", entry.first.c_str());
            return 2;
        }
    }
    for (const auto &entry : warningOverrides) warningOptions.set(entry.first, entry.second);
    includeDirs.insert(includeDirs.end(), project.includePaths.begin(), project.includePaths.end());
    importDirs.insert(importDirs.end(), project.includePaths.begin(), project.includePaths.end());
    importDirs.insert(importDirs.end(), project.importPaths.begin(), project.importPaths.end());
    includeDirs.insert(includeDirs.end(), project.searchPaths.begin(), project.searchPaths.end());
    importDirs.insert(importDirs.end(), project.searchPaths.begin(), project.searchPaths.end());
    importDirs.insert(importDirs.end(), moduleRuntimeDirs.begin(), moduleRuntimeDirs.end());
    /* `[chali] debug = true` 是项目级的“带调试节”，等价于命令行写 `--debug`。
     * 缺省 false（不带调试信息）；配置内容进缓存签名，改它会让缓存失效。 */
    if (project.debug) { stripDebug = false; debugEnabled = true; aotOptions.debug = true; }
    if (aotOptions.target.empty()) aotOptions.target = project.aotTarget;
    if (aotOptions.cpu.empty()) aotOptions.cpu = project.aotCpu;
    if (aotOptions.sysroot.empty()) aotOptions.sysroot = project.aotSysroot;
    if (aotOptions.runtimeDirectory.empty()) aotOptions.runtimeDirectory = project.aotRuntimeDirectory;
    if (aotOptions.allocator.empty()) aotOptions.allocator = project.aotAllocator;
    if (aotOptions.compiler.empty()) aotOptions.compiler = project.aotCompiler;
    if (aot && !run && !checkOnly && !aotOptions.emitLLVMOnly && !aotOptions.emitObjectOnly && !aotOptions.emitAssemblyOnly)
    {
        aotOptions.emitLLVMOnly = project.aotOutput == "llvm";
        aotOptions.emitObjectOnly = project.aotOutput == "object";
        aotOptions.emitAssemblyOnly = project.aotOutput == "assembly";
    }
    if (int(aotOptions.emitLLVMOnly) + int(aotOptions.emitObjectOnly) + int(aotOptions.emitAssemblyOnly) > 1)
    {
        std::fputs("chali: --emit-llvm、--emit-obj 与 --emit-asm 只能选择一种。\n", stderr);
        return 2;
    }
    if (aotOptions.emitLLVMOnly || aotOptions.emitObjectOnly || aotOptions.emitAssemblyOnly)
    {
        aot = true;
        run = 0;
    }
    chali_aot::TargetInfo selectedTarget;
    Memory::String targetError;
    if ((aot || run) && !chali_aot::describeTarget(aotOptions.target, selectedTarget, &targetError))
    {
        std::fprintf(stderr, "chali: %s\n", targetError.c_str());
        return 2;
    }
    if (run && !selectedTarget.native)
    {
        std::fputs("chali: run 只能执行宿主目标；请使用 aot 导出交叉编译产物。\n", stderr);
        return 2;
    }
    /* 构建**项目**时入口是 `[chali] main` 指定的主文件（命令行没给源文件才用它）；
     * 单文件编译就是命令行显式给的那个文件，不看主文件。
     * `projectMainPath` 只是为了让 `const char *path` 在整个编译期都有效。 */
    Memory::String projectMainPath;
    if (path == nullptr && !project.main.empty())
    {
        projectMainPath = project.main;
        path = projectMainPath.c_str();
    }
    if (path == nullptr && !readStdin)
    {
        PrintUsage(stdout);
        return 0;
    }
    Memory::UniquePtr<chali_incremental::Cache> cache;
    if (incremental)
    {
        Memory::Vector<Memory::String> searchDirs = includeDirs;
        searchDirs.insert(searchDirs.end(), importDirs.begin(), importDirs.end());
        Memory::String pathSignature = project.signature() + "\ninclude paths\n";
        for (const auto &entry : warningOptions.levels) pathSignature += entry.first + "=" + entry.second + "\n";
        if (warningOptions.errors) pathSignature += "warnings as errors\n";
        for (const auto &directory : includeDirs) pathSignature += chali_incremental::canonical(directory) + "\n";
        pathSignature += "import paths\n";
        for (const auto &directory : importDirs) pathSignature += chali_incremental::canonical(directory) + "\n";
        cache = Memory::makeUnique<chali_incremental::Cache>(cacheDirectory.empty() ? prefix + ".chali-cache" : cacheDirectory, argv[0],
                                                             searchDirs, defineItems,
                                                             pathSignature,
                                                             Memory::String(reflectionEnabled ? "reflection\n" : "no reflection\n") + (stripDebug ? "strip debug\n" : "keep debug\n") + "\n");
        cache->setOverlays(sourceOverlays);
    }

    const Memory::String sourceLabel = (path != nullptr) ? Memory::String(path) : Memory::String("<标准输入>");
    Memory::String text;
    if (path != nullptr && !sourceStdin)
    {
        std::ifstream stream(std::filesystem::u8path(path), std::ios::binary);
        if (!stream)
        {
            std::fprintf(stderr, CHALI_TEXT("不认识的文件：%s\n"), path);
            return 2;
        }
        text.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    }
    else
    {
        text.assign(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
    }

    const Memory::String originalRootSource = text;
    /* ---- 前处理：`%if` / `%define` / `%error` … ----
     *
     * 用 `forMetro()` 这套：只认 `%` 前缀，`%define` / `%undefine` 也算符号定义。
     * `preserveLines` 默认开着 —— 指令行与"条件为假"的行都输出成空行，所以
     * 预处理之后的**行号与源文件一一对应**（通过 lineMap 保留源位置）。
     * 模块通过 import 独立加载，命令行 -I 提供模块搜索目录。 */
    if (!libraryModule.empty() || !libraryVersion.empty())
    {
        if (!aot || run || checkOnly || raw || !outGiven || !chali_package::moduleName(libraryModule) || libraryVersion.empty())
        {
            std::fputs("chali lib: build 需要模块、精确包版本、--out 路径及实现文件\n", stderr);
            return 2;
        }
        if (path && std::filesystem::weakly_canonical(std::filesystem::u8path(path)) == std::filesystem::weakly_canonical(std::filesystem::u8path(prefix.c_str())))
        {
            std::fputs("chali lib: 输出不能覆盖实现源码\n", stderr);
            return 2;
        }
    }
    chali_report::clearVirtualSources();
    chali_library::ImportSession libraries(aotOptions.target.c_str(), aotOptions.cpu.c_str());
    if (!libraryLock.empty())
    {
        try
        {
            libraries.loadLock(std::filesystem::u8path(libraryLock.c_str()));
        } catch (const std::exception &failure)
        {
            std::fprintf(stderr, "chali: %s\n", failure.what());
            return 2;
        }
    }
    preprocess::Result processed;
    chali_macro::sourceFragments.clear();
    Memory::Vector<preprocess::LineOrigin> originalMacroOrigins;
    std::uint64_t incrementalInput = 0;
    if (!raw)
    {
        preprocess::Options options = preprocess::Preprocessor::forMetro();
        options.allowIncludes = false;
        options.includeDirs = includeDirs;
        if (!sourceOverlays.empty()) options.includeLoader = [&](const preprocess::IncludeRequest &request) {
            const auto found = sourceOverlays.find(chali_incremental::canonical(request.resolvedPath));
            return found == sourceOverlays.end() ? preprocess::IncludeResult::useDefault() :
                                                   preprocess::IncludeResult::inlineText(found->second, request.resolvedPath);
        };
        for (const Memory::String &item : defineItems)
        {
            preprocess::Define define;
            if (!preprocess::Preprocessor::parseDefine(item, define))
            {
                std::fprintf(stderr, "chali: -D 的写法不对（要 NAME 或 NAME=值）：%s\n",
                             item.c_str());
                return 2;
            }
            options.defines.push_back(define);
        }
        Memory::Vector<Memory::String> moduleDirs = importDirs;
        chali_module::Loader loader(options, moduleDirs, cache.get(), &project, &libraries);
        Memory::String moduleError;
        const bool ok = (path != nullptr && !sourceStdin) ? loader.rootFile(path, processed, moduleError) : loader.rootText(text, sourceLabel, path ? Memory::String(std::filesystem::path(path).parent_path().string()) : Memory::String("."), processed, moduleError);
        if (ok && !libraryModule.empty() && loader.rootModule() != libraryModule)
        {
            std::fprintf(stderr, "chali lib: 入口实际模块 %s 与请求模块 %s 不符\n", loader.rootModule().c_str(), libraryModule.c_str());
            return 1;
        }
        if (!libraryLock.empty()) processed.dependencies.push_back(std::filesystem::weakly_canonical(std::filesystem::u8path(libraryLock.c_str())).u8string().c_str());
        auto analysisFiles = processed.dependencies;
        if (path) analysisFiles.push_back(path);
        if (!project.file.empty())
        {
            analysisFiles.push_back(project.file);
            const auto lock = std::filesystem::path(project.file).parent_path() / "chali.lock";
            if (std::filesystem::exists(lock)) analysisFiles.push_back(lock.string());
        }
        chali_analysis::dependencies(analysisDependencies, analysisFiles, ok);
        for (const preprocess::Diagnostic &item : processed.diagnostics)
            if (!jsonDiagnostics) std::fprintf(stderr, CHALI_TEXT("chali: 前处理：%s\n"), item.toString().c_str());
        if (!ok)
        {
            if (jsonDiagnostics)
            {
                if (!processed.diagnostics.empty())
                {
                    const auto &d = processed.diagnostics.front();
                    chali_diagnostics::report("CHL1100", d.toString(), d.file, d.line);
                }
                else
                    chali_diagnostics::report("CHL1100", moduleError, sourceLabel);
                return 1;
            }
            std::fprintf(stderr, "chali: %s：%s\n",
                         loader.preprocessError() ? "前处理" : "模块",
                         moduleError.c_str());
            return 1;
        }

        /* C 宿主演示提供的普通 ChaLi 类型 / 接口声明追加在**合并源码末尾**：
         * 于是 `.chl` 里不必重复写 `struct` / `enum` / `interface` / `impl`。
         *
         * 只有程序里真的出现了这些名字才注入：类型号按出现顺序分配，凭空多出的声明会让
         * `__typeTag(...)` 的既有数值漂移（`example/samples/obj.chl` 就盯着这些数字）。没用到就一个
         * 字节都不加；用到时追加在末尾，也尽量少动编号。合成行只进 lineMap，报错时指向
         * `<胶水声明>`。 */
        Memory::String glueDeclarations;
        if (abi_demo_declarations_used(processed.text.data(), processed.text.size()))
            glueDeclarations = abi_demo_declarations(nullptr);
        if (!glueDeclarations.empty())
        {
            if (!processed.text.empty() && processed.text.back() != '\n')
            {
                processed.text += '\n';
                preprocess::LineOrigin pad;
                if (!processed.lineMap.empty()) pad = processed.lineMap.back();
                processed.lineMap.push_back(pad);
            }
            processed.text += glueDeclarations;
            std::size_t declarationLines = 0u;
            for (const char value : glueDeclarations)
                if (value == '\n') ++declarationLines;
            if (glueDeclarations.back() != '\n') ++declarationLines;
            for (std::size_t line = 1u; line <= declarationLines; ++line)
                processed.lineMap.push_back(preprocess::LineOrigin{ "<胶水声明>", line });
        }

        if (cache && !aot && run == 0 && !depsOnly && !checkOnly)
        {
            incrementalInput = chali_incremental::inputHash(processed, cache->signature());
            if (cache->outputsCurrent(prefix, incrementalInput, false))
            {
                std::fprintf(stderr, "chali: 增量缓存命中：%zu 个模块复用，编译产物未变\n",
                             cache->reused());
                if (runtimeStats) std::fprintf(stderr, "chali: 增量文件校验：%zu 次读取，%zu 次复用\n", cache->hashReads(), cache->hashReuses());
                return 0;
            }
        }
        Memory::String macroError;
        std::string syntaxError;
        originalMacroOrigins = processed.lineMap;
        if (!chali_macro::markReplacementBoundaries(processed.text, syntaxError))
        {
            std::fprintf(stderr, "chali: %s\n", syntaxError.c_str());
            return 1;
        }
        if (!chali_macro::sourceFragments.capture(processed.text, syntaxError, 1, 1, true))
        {
            std::fprintf(stderr, "chali: %s\n", syntaxError.c_str());
            return 1;
        }
        if (!chali_interpolation::run(processed, macroError))
        {
            std::fprintf(stderr, "chali: 插值：%s\n", macroError.c_str());
            return 1;
        }
        chali_macro::ProtectedDefinitions definitions;
        if (!definitions.hide(processed.text, syntaxError))
        {
            if (jsonDiagnostics)
            {
                chali_diagnostics::report("CHL1100", Memory::String(syntaxError.data(), syntaxError.size()), sourceLabel);
                return 1;
            }
            std::fprintf(stderr, "chali: %s\n", syntaxError.c_str());
            return 1;
        }
        if (!chali_lambda::run(processed, macroError))
        {
            if (jsonDiagnostics)
            {
                chali_diagnostics::report("CHL1100", macroError, sourceLabel);
                return 1;
            }
            std::fprintf(stderr, "chali: lambda：%s\n", macroError.c_str());
            return 1;
        }
        chali_generic::setTemplateLoader(standardUnit && buildStandardUnit.empty() && buildStandardModule.empty() && buildStandardTemplates.empty()
                                          ? chali_precompiled::loadTemplate : nullptr);
        if (!chali_generic::run(processed, macroError))
        {
            if (jsonDiagnostics)
            {
                chali_diagnostics::report("CHL1500", macroError, sourceLabel);
                return 1;
            }
            std::fprintf(stderr, CHALI_TEXT("chali: 泛型：%s\n"), macroError.c_str());
            return 1;
        }
        chali_lambda::markClosureTypeArguments(processed.text);
        if (!definitions.restore(processed.text, syntaxError))
        {
            std::fprintf(stderr, "chali: %s\n", syntaxError.c_str());
            return 1;
        }
        text = processed.text;
        const auto originOffset = processed.lineMap.size();
        processed.lineMap.insert(processed.lineMap.end(), originalMacroOrigins.begin(), originalMacroOrigins.end());
        chali_macro::sourceFragments.rebase(originOffset);
        if (depsOnly)
        {
            for (const Memory::String &file : processed.dependencies) std::printf("%s\n", file.c_str());
            return 0;
        }
    }

    Memory::Vector<Memory::String> sourceFiles;
    sourceFiles.reserve(processed.lineMap.size());
    for (const auto &origin : processed.lineMap) sourceFiles.push_back(origin.file);
    Memory::Vector<Memory::String> librarySources;
    for (const auto &file : libraries.sources()) librarySources.push_back(Memory::String(file.c_str()));
    M_SetLibrarySources(librarySources);
    M_SetSourceFiles(sourceFiles, sourceLabel);
    Memory::Vector<std::size_t> sourceLines;
    for (const auto &origin : processed.lineMap) sourceLines.push_back(origin.line);
    M_SetSourceLines(sourceLines);
    Engine engine;
    ParserValue_T value{};
    std::string fragmentError;
    if (raw && !chali_macro::markReplacementBoundaries(text, fragmentError))
    {
        std::fprintf(stderr, "chali: %s\n", fragmentError.c_str());
        return 1;
    }
    if (!chali_macro::sourceFragments.capture(text, fragmentError, 1, 1, true))
    {
        std::fprintf(stderr, "chali: %s\n", fragmentError.c_str());
        return 1;
    }
    chali_lambda::markClosureTypeArguments(text);
    chali_generic::markContainerTypeArguments(text);
    chali_macro::markHeaderBoundaries(text);
    if (!buildStandardTemplates.empty())
    {
        std::ofstream output(Memory::u8path(buildStandardTemplatesOutput), std::ios::binary | std::ios::trunc);
        if (raw || !output || !chali_precompiled::writeTemplates(output, buildStandardTemplates))
        {
            std::fprintf(stderr, "chali: 标准泛型模板二进制生成失败：%s\n", buildStandardTemplates.c_str());
            return 1;
        }
        return 0;
    }
    const auto dependencyEnd = chali_module::findStandardBoundary(text);
    const auto optionalUnits = chali_precompiled::moduleSlices(text, sourceFiles,
        dependencyEnd == Memory::String::npos ? 0 : dependencyEnd);
    if (!buildStandardModule.empty())
    {
        const auto found = std::find_if(optionalUnits.begin(), optionalUnits.end(), [&](const auto &unit) { return unit.name == buildStandardModule; });
        if (raw || found == optionalUnits.end())
        {
            std::fprintf(stderr, "chali: 无法定位标准模块资产：%s\n", buildStandardModule.c_str());
            return 1;
        }
        const auto moduleText = text.substr(found->begin, found->end - found->begin);
        const auto padding = static_cast<std::size_t>(std::count(text.begin(), text.begin() + found->begin, '\n'));
        if (engine.runText(Memory::String(padding, '\n') + moduleText, &value) != Status::Ok)
        {
            std::fprintf(stderr, "chali: 标准模块资产解析失败：%s\n", buildStandardModule.c_str());
            return 1;
        }
        std::ofstream output(Memory::u8path(buildStandardModuleOutput), std::ios::binary | std::ios::trunc);
        if (!output || !chali_precompiled::writeModule(output, moduleText))
        {
            std::fprintf(stderr, "chali: 标准模块二进制生成失败：%s\n", buildStandardModule.c_str());
            return 1;
        }
        return 0;
    }
    /* 核心先恢复；可选标准模块按原导入顺序追加，用户依赖和实例尾部仍独立解析。 */
    std::size_t standardModuleHits = 0, standardModuleMisses = 0;
    std::size_t boundary = Memory::String::npos;
    std::size_t standardLines = 0;
    std::size_t firstUserNode = 0;
    std::size_t errorLineOffset = 0;      ///< 解析期报错的行号偏移：第二段解析出来的行号要加回标准库的行数

    bool standardUnitLoaded = false;
    if ((standardUnit || !buildStandardUnit.empty()) && !raw)
    {
        boundary = chali_module::findStandardCoreBoundary(text);
        if (boundary != Memory::String::npos)
        {
            for (std::size_t i = 0; i < boundary; ++i)
                if (text[i] == '\n') ++standardLines;
            if (buildStandardUnit.empty())
            {
                Memory::String error;
                standardUnitLoaded = chali_precompiled::load(text.substr(0, boundary), error);
                if (!error.empty())
                {
                    std::fprintf(stderr, "chali: %s\n", error.c_str());
                    return 1;
                }
            }
        }
    }
    Status status = Status::Ok;
    if (boundary == Memory::String::npos)
    {
        status = engine.runText(text, &value);
    }
    else
    {
        if (!standardUnitLoaded) status = engine.runText(text.substr(0, boundary), &value);
        /* 标准库段自己解析出来的行号就是合并源码里的行号，不需要偏移。 */
        errorLineOffset = 0;
        firstUserNode = M_NodeCount();
        if (status == Status::Ok && !buildStandardUnit.empty())
        {
            std::ofstream output(std::filesystem::u8path(buildStandardUnit.begin(), buildStandardUnit.end()), std::ios::binary | std::ios::trunc);
            if (!output || !chali_precompiled::write(output, text.substr(0, boundary), firstUserNode))
            {
                std::fputs(CHALI_TEXT("chali: 无法生成标准库二进制预编译资产\n"), stderr);
                return 1;
            }
            return 0;
        }
        if (status == Status::Ok)
        {
            /* 用前导空行保留绝对坐标，原始宏调用来源不再经过重复平移。 */
            const auto parsePart = [&](std::size_t begin, std::size_t end) {
                if (begin >= end || status != Status::Ok) return;
                const auto prefix = M_TopLevelIds();
                const auto padding = static_cast<std::size_t>(std::count(text.begin(), text.begin() + begin, '\n'));
                status = engine.runText(Memory::String(padding, '\n') + text.substr(begin, end - begin), &value);
                M_PrefixTopLevel(prefix);
                errorLineOffset = 0;
            };
            std::size_t cursor = boundary;
            for (const auto &unit : optionalUnits)
            {
                if (unit.begin < cursor) continue;
                parsePart(cursor, unit.begin);
                if (status != Status::Ok) break;
                Memory::String error;
                if (chali_precompiled::loadModule(unit.name, text.substr(unit.begin, unit.end - unit.begin), error))
                {
                    ++standardModuleHits;
                    if (runtimeStats) std::fprintf(stderr, "标准模块二进制命中：%s\n", unit.name.c_str());
                }
                else
                {
                    if (!error.empty())
                    {
                        std::fprintf(stderr, "chali: %s：%s\n", unit.name.c_str(), error.c_str());
                        return 1;
                    }
                    ++standardModuleMisses;
                    parsePart(unit.begin, unit.end);
                }
                cursor = unit.end;
            }
            parsePart(cursor, text.size());
        }
    }
    if (status != Status::Ok)
    {
        const Error *error = engine.lastError();
        if (jsonDiagnostics)
        {
            const auto line = error ? error->line() + errorLineOffset : 1;
            const auto original = line && line <= processed.lineMap.size() ? processed.lineMap[line - 1] : preprocess::LineOrigin{ sourceLabel, line };
            /* JSON 诊断（编辑器里划波浪线）也走一遍呈现层：否则编辑器显示的是
             * `SEMI` / `IMPL` 这种内部名，跟终端里看到的两套说法。 */
            chali_diagnostics::report("CHL1001",
                                      error ? chali_report::localizeSyntaxMessage(error->message()) : Memory::String(CHALI_TEXT("解析失败")),
                                      original.file, original.line, error ? error->column() : 1);
            return 1;
        }
        if (error != nullptr)
        {
            /* 行号是**预处理之后**的行号：有映射就翻回源文件（模块展开的位置
             * 尤其需要 —— 报错要指向真正写错的那个文件）。 */
            const std::size_t line = error->line() + errorLineOffset;
            /* 消息里写的是 Token 内部名（`SEMI` / `IMPL`…），换成「分号(;)」这种
             * 用户看得懂的说法；外层已经写了「语法错误：」，消息里那句前缀要剥掉。
             * 整句骨架也按当前显示语言重建（英文是 `expected … , found …` 的语序）。 */
            const Memory::String message =
                chali_report::localizeSyntaxMessage(error->message());
            if (line >= 1u && line <= processed.lineMap.size() && !processed.lineMap[line - 1u].file.empty())
            {
                const preprocess::LineOrigin &origin = processed.lineMap[line - 1u];
                std::fprintf(stderr, CHALI_TEXT("语法错误：%s 第 %zu 行第 %u 列：%s\n"), origin.file.c_str(),
                             origin.line, error->column(), message.c_str());
                /* 回显出错那一行 + 波浪号（长度取出错 Token 的词素长度）。 */
                const Memory::String snippet = chali_report::renderSnippet(
                    origin.file, origin.line, error->column(), error->tokenText().size());
                if (!snippet.empty()) std::fputs(snippet.c_str(), stderr);
            }
            else
            {
                std::fprintf(stderr, CHALI_TEXT("语法错误：第 %u 行第 %u 列：%s\n"), error->line(),
                             error->column(), message.c_str());
            }
        }
        else
        {
            std::fprintf(stderr, CHALI_TEXT("解析失败：%s\n"), statusName(status));
        }
        return 1;
    }

    Memory::Vector<const char *> argumentPointers;
    argumentPointers.reserve(programArguments.size());
    for (const Memory::String &argument : programArguments)
        argumentPointers.push_back(argument.c_str());
    M_SetProgramArguments(static_cast<int>(argumentPointers.size()), argumentPointers.data());
    M_SetThreadsEnabled(threadsEnabled ? 1 : 0);
    M_SetDebugEnabled(debugEnabled ? 1 : 0);
    M_SetStepLimit(stepLimit);
    M_SetRuntimeMemory(memoryBytes);
    M_SetCheckOnly(checkOnly);
    M_SetSharedLibrary(aot && aotOptions.shared);
    chali_editor::compilerAnalysis.begin(!analysisTypes.empty());
    const int code = M_Compile("chali", prefix.c_str(), 0, 0, nullptr);
    if (!analysisTypes.empty())
    {
        std::string error;
        if (!chali_editor::compilerAnalysis.write(std::string(analysisTypes.data(), analysisTypes.size()), code == 0, error))
        {
            std::fprintf(stderr, "chali: %s\n", error.c_str());
            return 1;
        }
    }
    Memory::Vector<chali_diagnostics::Diagnostic> warnings;
    for (const auto &item : M_Warnings())
    {
        const auto original = item.line && item.line <= processed.lineMap.size() ? processed.lineMap[item.line - 1] : preprocess::LineOrigin{ sourceLabel, item.line };
        warnings.push_back({ item.message, original.file, original.line, item.column ? item.column : 1u, true });
    }
    if (code == 0)
    {
        auto candidates = warningOptions.level("CHL2201") == "allow" ? Memory::Vector<chali_diagnostics::Diagnostic>() : chali_warnings::collect(M_AotSyntax());
        warnings.insert(warnings.end(), candidates.begin(), candidates.end());
        const std::regex typeName("[A-Z][a-z0-9]*([A-Z][a-z0-9]+)*"), methodName("[a-z][A-Za-z0-9]*");
        for (std::size_t i = 0; i < M_NodeCount(); ++i)
        {
            const auto &node = M_ParsedNode(static_cast<int>(i));
            const bool type = node.kind == MNode::StructDecl || node.kind == MNode::EnumDecl || node.kind == MNode::InterfaceDecl || node.kind == MNode::OpcodeDecl;
            if (warningOptions.level(type ? "CHL2101" : "CHL2102") == "allow") continue;
            if ((!type && node.kind != MNode::FuncDef && node.kind != MNode::MethodDef) || node.macroExpansion >= 0 || node.nameParameter || node.s.empty()) continue;
            auto name = node.s;
            const auto scope = name.rfind("::");
            if (scope != name.npos) name = name.substr(scope + 2);
            if (name.empty() || name[0] == '_' || name.find('<') != name.npos || name.find('$') != name.npos || (node.kind == MNode::MethodDef && node.type2 == 2)) continue;
            if (std::regex_match(std::string(name.data(), name.size()), type ? typeName : methodName)) continue;
            const auto origin = node.sourceLine && node.sourceLine <= processed.lineMap.size() ? processed.lineMap[node.sourceLine - 1] : preprocess::LineOrigin{ sourceLabel, node.sourceLine };
            chali_diagnostics::Diagnostic d;
            d.message = (type ? Memory::String("类型名建议使用大驼峰（PascalCase）：") : Memory::String("函数名建议使用小驼峰（camelCase）：")) + name;
            d.file = origin.file;
            d.line = origin.line;
            d.id = type ? "CHL2101" : "CHL2102";
            warnings.push_back(std::move(d));
        }
    }
    Memory::Map<Memory::String, Memory::String> warningSources;
    const auto originalBody = [&](const Memory::String &file) -> const Memory::String & {
        auto found = warningSources.find(file);
        if (found != warningSources.end()) return found->second;
        Memory::String body;
        const auto canonical = chali_incremental::canonical(file);
        if (canonical == chali_incremental::canonical(sourceLabel))
            body = originalRootSource;
        else if (sourceOverlays.count(canonical))
            body = sourceOverlays.at(canonical);
        else
        {
            std::ifstream input(Memory::u8path(file), std::ios::binary);
            body.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        }
        return warningSources.emplace(file, std::move(body)).first->second;
    };
    Memory::Vector<chali_diagnostics::Diagnostic> filteredWarnings;
    bool deniedWarning = false;
    Memory::Set<Memory::String> seenWarnings;
    for (auto &item : warnings)
    {
        if (item.file.empty() || item.file[0] == '<') continue;
        if (!chali_warnings::prepare(item, warningOptions, originalBody(item.file))) continue;
        if (!seenWarnings.insert(item.file + ":" + std::to_string(item.line).c_str() + ":" + item.id + ":" + item.message).second) continue;
        deniedWarning |= !item.warning;
        if (!jsonDiagnostics)
        {
            std::fprintf(stderr, "chali: %s：%s:%zu:%zu：%s%s%s\n", item.warning ? "警告" : "错误", item.file.c_str(), item.line, item.column,
                         item.message.find("[" + item.id + "]") == item.message.npos ? ("[" + item.id + "] ").c_str() : "", item.message.c_str(), "");
            const auto snippet = chali_report::renderSnippet(item.file, item.line, item.column);
            if (!snippet.empty()) std::fputs(snippet.c_str(), stderr);
        }
        filteredWarnings.push_back(std::move(item));
    }
    warnings.swap(filteredWarnings);
    if (code == 0 && deniedWarning)
    {
        if (jsonDiagnostics) chali_diagnostics::reportMany(warnings);
        return 1;
    }
    /* 后端两层的统计：指令选择覆盖率与帧槽复用前后。
     * 它们都只在编译期累加，和运行期内存统计一起挂在 `--stats` 下。 */
    if (runtimeStats)
    {
        const stackvm::execution_detail::SelectionStats selectionStats = stackvm::selection::lastStats();
        if (selectionStats.total != 0u)
            std::fprintf(stderr, "%s\n", stackvm::selection::describe(selectionStats).c_str());
        const stackvm::allocation::Stats allocationStats = stackvm::allocation::lastStats();
        if (allocationStats.functions != 0u)
            std::fprintf(stderr, "帧槽分配：%zu 个函数，符号 %zu 个 → 槽 %zu 个（省掉 %zu 个）\n",
                         allocationStats.functions, allocationStats.symbols, allocationStats.slots,
                         allocationStats.symbols - std::min(allocationStats.symbols, allocationStats.slots));
    }
    if (runtimeStats)
    {
        if (boundary == Memory::String::npos)
            std::fprintf(stderr, "标准库预编译：未启用（%s）\n", standardUnit ? "没有标准库分界" : "已用 --no-stdlib-cache 关闭");
        else if (standardUnitLoaded)
            std::fprintf(stderr, "标准库预编译：命中内嵌核心（%zu 个节点，省掉核心解析）\n", firstUserNode);
        else
            std::fprintf(stderr, "标准库预编译：源码解析（%zu 个节点，%zu 行；不写磁盘缓存）\n", firstUserNode, standardLines);
        if (standardUnit && !optionalUnits.empty())
            std::fprintf(stderr, "标准模块二进制：%zu 命中，%zu 源码解析\n", standardModuleHits, standardModuleMisses);
        if (standardUnit)
            std::fprintf(stderr, "标准模板二进制：%zu 命中，%zu 源码收集\n", chali_generic::session.templateHits, chali_generic::session.templateMisses);
    }
    if (code != 0)
    {
        Memory::Vector<chali_diagnostics::Diagnostic> diagnostics;
        for (auto item : M_Diagnostics())
        {
            item.message = MacroDiagnosticDisplay(item.message, macroBacktraceFull);
            const auto original = item.line && item.line <= processed.lineMap.size() ? processed.lineMap[item.line - 1] : preprocess::LineOrigin{ sourceLabel, item.line };
            diagnostics.push_back({ item.message, original.file, original.line, item.column ? item.column : 1u });
            diagnostics.back().bindSource(originalBody(original.file));
            if (!jsonDiagnostics)
            {
                /* `<内建 lang>` 这类合成文件是编译器自己拼出来的源码（前处理的行映射会翻
                 * 到它），行号对用户没有意义；真正读不到原文（renderSnippet 返回空）也一样，
                 * 那就维持原来"只有一句话"的写法 —— 宁可不给位置，也不给一个错的位置。 */
                const bool positioned = item.line && !original.file.empty() && original.file.front() != '<' && chali_report::hasSourceLine(original.file, original.line);
                if (positioned)
                {
                    /* 语义 / 类型 / 借用诊断也要给"报错源提醒"：先写 `文件 第 N 行[第 M 列]`，
                     * 再回显那一行原文 + 波浪号。列只有注解那种贴着 token 的检查才给得出，
                     * 拿不到就只写行、整行铺波浪号（不能什么都不写 —— 用户 copy 不到出错位置）。 */
                    /* 自动落点只会给到语句最后那个 token，所以要往前挪到语句开头；
                     * 这笔账算在这里 —— 文案里的"第 N 列"与波浪号必须是同一个位置。 */
                    const std::size_t column = item.column ? chali_report::anchoredColumn(original.file, original.line, item.column) : 0u;
                    if (column)
                        std::fprintf(stderr, CHALI_TEXT("chali: %s 第 %zu 行第 %zu 列：%s\n"),
                                     original.file.c_str(), original.line, column, item.message.c_str());
                    else
                        std::fprintf(stderr, CHALI_TEXT("chali: %s 第 %zu 行：%s\n"),
                                     original.file.c_str(), original.line, item.message.c_str());
                    const Memory::String snippet = chali_report::renderSnippet(
                        original.file, original.line, column,
                        column ? 0u : chali_report::kSnippetWholeLine);
                    if (!snippet.empty()) std::fputs(snippet.c_str(), stderr);
                }
                else
                    std::fprintf(stderr, "chali: %s\n", item.message.c_str());
            }
        }
        if (diagnostics.empty() && M_LastError() && M_LastError()[0])
        {
            const auto message = MacroDiagnosticDisplay(M_LastError(), macroBacktraceFull);
            const auto line = M_LastErrorLine();
            const auto column = M_LastErrorColumn();
            const auto original = line && line <= processed.lineMap.size() ? processed.lineMap[line - 1] : preprocess::LineOrigin{ sourceLabel, line };
            diagnostics.push_back({ message, original.file, original.line, column ? column : 1u });
            diagnostics.back().bindSource(originalBody(original.file));
            if (!jsonDiagnostics)
            {
                /* 行号为 0 = 这条错误没有可指的源码位置（比如运行期错误：位置得跑起来才知道，
                 * 它的 `出错位置` / `调用栈` 自己就写在下面几行）。这时保持单行原文不加前缀。 */
                if (!line || original.file.empty() || original.file.front() == '<' || !chali_report::hasSourceLine(original.file, original.line))
                    std::fprintf(stderr, "chali: %s\n", message.c_str());
                else
                {
                    const std::size_t anchored = column ? chali_report::anchoredColumn(original.file, original.line, column) : 0u;
                    if (anchored)
                        std::fprintf(stderr, CHALI_TEXT("chali: %s 第 %zu 行第 %zu 列：%s\n"),
                                     original.file.c_str(), original.line, anchored, message.c_str());
                    else
                        std::fprintf(stderr, CHALI_TEXT("chali: %s 第 %zu 行：%s\n"),
                                     original.file.c_str(), original.line, message.c_str());
                    const Memory::String snippet = chali_report::renderSnippet(
                        original.file, original.line, anchored,
                        anchored ? 0u : chali_report::kSnippetWholeLine);
                    if (!snippet.empty()) std::fputs(snippet.c_str(), stderr);
                }
            }
        }
        diagnostics.insert(diagnostics.end(), warnings.begin(), warnings.end());
        if (jsonDiagnostics) chali_diagnostics::reportMany(diagnostics);
        return code;
    }
    if (aot && !checkOnly)
    {
        Memory::Vector<Memory::String> diagnostics;
        chali_aot::Request request;
        request.syntax = M_AotSyntax();
        request.externs = chali_aot::collectAbiExterns();
        request.abiGlue = chali_aot::collectAbiGlue();
        request.abiExports = chali_aot::collectAbiExports();
        request.abiExportHeader = chali_aot::collectAbiExportHeader();
        for (auto &name : aotOptions.exports)
        {
            bool exact = false, implicit = false;
            if (request.syntax) for (const auto &function : request.syntax->functions)
            {
                exact |= function.quadLabel() == name;
                implicit |= function.quadLabel() == "program::" + name;
            }
            if (!exact && implicit) name = "program::" + name;
        }
        for (const auto &label : M_AotExports())
            if (std::find(aotOptions.exports.begin(), aotOptions.exports.end(), label) == aotOptions.exports.end())
                aotOptions.exports.push_back(label);
        /* 缺省导出物放在源文件同级（Windows 带 .exe，其他平台无后缀）；中间产物仍在 .chali/ 下。 */
        Memory::String exportPrefix = prefix;
        if (!run && !outGiven && path != nullptr)
        {
            const std::filesystem::path sourcePath = std::filesystem::u8path(path);
            const std::filesystem::path directory = sourcePath.parent_path();
            const std::filesystem::path stem = std::filesystem::u8path(sourcePath.stem().u8string());
            exportPrefix = (directory.empty() ? stem : directory / stem).u8string();
        }
        aotOptions.outputPrefix = exportPrefix;
        aotOptions.memoryBytes = memoryBytes;
        aotOptions.stepLimit = stepLimit;
        aotOptions.threadsEnabled = threadsEnabled;
        for (const auto &external : request.externs)
            if (request.abiGlue.empty() && !external.library.empty() && !chali_aot::isDirectLinkLibrary(external.library) && !chali_aot::usesDynamicAbiLibrary(external) && std::find(aotOptions.linkLibraries.begin(), aotOptions.linkLibraries.end(), external.library) == aotOptions.linkLibraries.end())
                aotOptions.linkLibraries.push_back(external.library);
        try
        {
            libraries.appendTo(aotOptions);
        } catch (const std::exception &failure)
        {
            std::fprintf(stderr, "chali: %s\n", failure.what());
            return 1;
        }
        request.options = aotOptions;
        if (!libraryModule.empty())
        {
            try
            {
                const auto output = std::filesystem::weakly_canonical(std::filesystem::u8path(exportPrefix.c_str()));
                for (const auto &input : libraries.paths())
                    if (input == output) throw std::runtime_error("库输出不能覆盖导入的库包");
                if (!libraryLock.empty() && output == std::filesystem::weakly_canonical(std::filesystem::u8path(libraryLock.c_str())))
                    throw std::runtime_error("库输出不能覆盖内容锁");
                const auto interface = M_LibraryInterface(libraryModule, request.abiExports);
                return chali_library::buildNative(request, libraryModule.c_str(), libraryVersion.c_str(),
                                                  std::string(interface.data(), interface.size()), libraries.records(), output);
            } catch (const std::exception &failure)
            {
                std::fprintf(stderr, "chali lib: %s\n", failure.what());
                return 1;
            }
        }
        const int status = chali_aot::run(request, &diagnostics);
        for (const auto &message : diagnostics) std::fprintf(stderr, "chali: %s\n", message.c_str());
        if (status != 0) return status;
        /* 缺省运行或导出完成后清理中间产物；--out 显式给路径时按用户要求全部保留，
         * --keep-artifacts、--incremental 与 --emit-llvm 同样保留。 */
        const bool cleanArtifacts = !outGiven && !keepArtifacts && !incremental && !aotOptions.emitLLVMOnly && !aotOptions.emitObjectOnly && !aotOptions.emitAssemblyOnly;
        /* 带 `--export` 时 ABI 头是交给宿主代码的交付物，不能当中间产物删掉。 */
        const bool withHeader = aotOptions.exports.empty();
        if (run)
        {
            const int exitStatus = chali_aot::executeNative(request.options, programArguments);
            if (cleanArtifacts) RemoveIntermediateArtifacts(prefix, true, withHeader);
            return exitStatus;
        }
        if (cleanArtifacts)
        {
            /* 四元式产物留在 prefix（.chali/）下，LLVM 侧产物跟着导出前缀走，两边都要清。 */
            RemoveIntermediateArtifacts(prefix, false, withHeader);
            if (exportPrefix != prefix) RemoveIntermediateArtifacts(exportPrefix, false, withHeader);
        }
    }
    if (jsonDiagnostics) chali_diagnostics::reportMany(warnings);
    if (cache && run == 0 && !checkOnly)
    {
        cache->saveOutputs(prefix, incrementalInput, false);
        std::fprintf(stderr, "chali: 增量编译：%zu 个模块复用，%zu 个模块重新预处理\n",
                     cache->reused(), cache->rebuilt());
        if (runtimeStats) std::fprintf(stderr, "chali: 增量文件校验：%zu 次读取，%zu 次复用\n", cache->hashReads(), cache->hashReuses());
    }
    return 0;
} catch (const std::bad_alloc &)
{
    // 失败快照不使用池内存，耗尽后也能保留真实的容量、活动量与申请量。
    const auto failure = Memory::allocationFailureSignal().peek();
    std::fprintf(stderr, CHALI_TEXT("chali: 编译器内存预算不足（池容量 %zu 字节，失败时活动 %zu 字节，本次申请 %zu 字节）\n"),
                 failure.capacity, failure.liveBytes, failure.requestedBytes);
    return 1;
}

/* 独立编译器工程（非统一构建）自带 main；统一构建由 `chali_tool.cpp` 提供 main 并
 * 直接调用上面的 `chali_main_entry`，所以在 chali 那套构建里定义
 * `CHALI_FRONT_END_NO_MAIN` 把这里的 main 关掉。 */
#ifndef CHALI_FRONT_END_NO_MAIN
/**
 * @brief 调用 ChaLi 前端入口并返回其退出状态。
 * @param argc 命令行参数数量。
 * @param argv 命令行参数指针数组。
 * @retval 进程退出状态，0 表示成功，非零表示失败。
 */
int main(int argc, char **argv) { return chali_main_entry(argc, argv); }
#endif
