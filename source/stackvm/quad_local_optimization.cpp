/**
 * @file quad_local_optimization.cpp
 * @brief 四元式字面量折叠、局部改写和纯函数判断。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
namespace runtimeDetail
{

/**
 * @brief 在基本块内用值身份匹配重复地址，并转发最后一次完整整数写入。
 * @param code 四元式序列。
 * @param declared 已声明类型。
 * @param report 接收复制传播数量。
 * @return 是否发生转发。
 * @note 不删除或外提检查与运算；地址表达式仍在原位置求值。
 */
bool optimizeMemoryForwarding(Memory::Vector<Quad> &code, const Memory::Map<Memory::String, TypeInfo> &declared,
                              OptimizationReport &report)
{
    Memory::Map<Memory::String, std::size_t> values, expressions;
    Memory::Set<Memory::String> integers;
    std::size_t next = 0, storedAddress = 0, storedValue = 0;
    Memory::String storedSource;
    bool changed = false;
    const auto identity = [&](const Memory::String &name) {
        const auto found = values.find(name);
        if (found != values.end()) return found->second;
        const auto id = ++next;
        values.emplace(name, id);
        return id;
    };
    const auto integer = [&](const Memory::String &name) {
        const auto found = declared.find(name);
        if (found != declared.end()) return !found->second.isFloat;
        if (integers.count(name)) return true;
        bool numeric = false, floating = false;
        std::int64_t value = 0;
        double real = 0;
        Memory::String text;
        return readLiteral(name, numeric, floating, value, real, text) && numeric && !floating;
    };
    for (auto &quad : code)
    {
        Memory::String type;
        auto base = splitType(quad.op, type);
        const bool word = type == "i64" || type == "u64";
        if (base == "store")
        {
            storedSource.clear();
            if (word && integer(quad.arg(1)))
            {
                storedAddress = identity(quad.arg(0));
                storedSource = quad.arg(1);
                storedValue = identity(storedSource);
            }
            continue;
        }
        if (base == "load" && word && !storedSource.empty() && identity(quad.arg(0)) == storedAddress && identity(storedSource) == storedValue)
        {
            quad.op = ":=";
            quad.args = { storedSource, "_", quad.arg(2) };
            base = ":=";
            type.clear();
            ++report.copiesPropagated;
            changed = true;
        }
        if (base == "check" || base == "checkptr") continue;
        const bool arithmetic = base == "+" || base == "-" || base == "*" || base == "&" || base == "|" || base == "^" || base == "<<" || base == ">>";
        if (base == "decl" || base == ":=" || base == "load" || (arithmetic && word))
        {
            const auto destination = quad.arg(2);
            std::size_t result = ++next;
            const auto target = declared.find(destination);
            bool numeric = base == "decl" && target != declared.end() && !target->second.isFloat;
            const bool fullTarget = target == declared.end() || (!target->second.isFloat && target->second.bits == 64);
            if (base == ":=" && integer(quad.arg(0)) && fullTarget)
            {
                result = identity(quad.arg(0));
                numeric = true;
            }
            else if (arithmetic && fullTarget && integer(quad.arg(0)) && integer(quad.arg(1)))
            {
                const auto key = base + type + ":" + Memory::toString(identity(quad.arg(0))) + ":" + Memory::toString(identity(quad.arg(1)));
                const auto found = expressions.find(key);
                if (found == expressions.end())
                    expressions.emplace(key, result);
                else
                    result = found->second;
                numeric = true;
            }
            else if (base == "load" && word)
                numeric = true;
            values[destination] = result;
            if (numeric)
                integers.insert(destination);
            else
                integers.erase(destination);
            continue;
        }
        // 未识别的指令可能写内存、调用宿主或进入其他路径，不能继承地址和值事实。
        values.clear();
        expressions.clear();
        integers.clear();
        storedSource.clear();
        if (base == "alloc" || base == "addr")
        {
            values[quad.arg(2)] = ++next;
            integers.insert(quad.arg(2));
        }
    }
    return changed;
}

/**
 * @brief 读一个字面量。
 * @param token 原文。
 * @param isNumber 输出：是不是数字。
 * @param isFloat 输出：是不是浮点。
 * @param intValue 输出：整数值。
 * @param doubleValue 输出：浮点值。
 * @param text 输出：字符串内容。
 * @return 是字面量返回 true。
 */
bool readLiteral(const Memory::String &token, bool &isNumber, bool &isFloat,
                 std::int64_t &intValue, double &doubleValue, Memory::String &text)
{
    if (token.empty() || token == "_") return false;
    if (token.front() == '"' || token.front() == '\'')
    {
        isNumber = false;
        text = unquote(token);
        return true;
    }
    return parseNumber(token, isFloat, intValue, doubleValue) ? (isNumber = true, true) : false;
}

/**
 * @brief 一条四元式读到的变量（用于死代码分析）。
 * @param quad 四元式。
 * @return 变量名列表（不含字面量）。
 */
Memory::Vector<Memory::String> readOperands(const Quad &quad)
{
    Memory::String typeName;
    const Memory::String base = splitType(quad.op, typeName);
    Memory::Vector<Memory::String> names;

    /**
     * 收一个可能读到的操作数。
     * @param token 原文。
     */
    auto take = [&](const Memory::String &token) {
        if (token.empty()) return;
        bool isNumber = false, isFloat = false;
        std::int64_t intValue = 0;
        double doubleValue = 0.0;
        Memory::String text;
        if (readLiteral(token, isNumber, isFloat, intValue, doubleValue, text)) return;
        names.push_back(token);
    };

    if (base == "decl" || base == "label" || base == "call" || base == "j" || base == "halt" || base == "addr" || base == "print") return names;
    if (base == ":=" || base == "neg" || base == "~" || base == "cvt" || base == "write" || base == "putc" || base == "param" || base == "ret" || base == "jz" || base == "jnz")
    {
        take(quad.arg(0));
        return names;
    }
    if (base == "mul" || base == "div")
    {
        take(quad.arg(0));
        take(quad.arg(1));
        return names;
    }
    if (base == "memcpy" || base == "memset") take(quad.arg(2));
    // 二元运算与比较：前两个操作数。
    take(quad.arg(0));
    take(quad.arg(1));
    return names;
}

/**
 * @brief 这条四元式会不会写变量（写的是第几个参数）。
 * @param base 基础操作符。
 * @return 写入的参数下标；不写返回 -1。
 */
int writesTo(const Memory::String &base)
{
    if (base == ":=" || base == "neg" || base == "~" || base == "cvt" || base == "==" || base == "!=" || base == "<" || base == "<=" || base == ">" || base == ">=" || base == "mul" || base == "div" || base == "read" || base == "getc")
    {
        return 2;
    }
    if (base == "+" || base == "-" || base == "*" || base == "/" || base == "%" || base == "&" || base == "|" || base == "^" || base == "<<" || base == ">>")
    {
        return 2;
    }
    return -1;
}

/**
 * @brief 返回真正读取值的操作数下标，元数据及标签不参与传播。
 * @param base 基础操作码。
 * @return 读操作数下标。
 */
std::array<std::size_t, 3> optimizationReads(const Memory::String &base)
{
    const auto unused = static_cast<std::size_t>(-1);
    if (base == "memcpy" || base == "memset") return { 0, 1, 2 };
    if (base == "decl" || base == "label" || base == "call" || base == "j" || base == "halt" || base == "addr" || base == "print") return { unused, unused, unused };
    if (base == ":=" || base == "cvt" || base == "neg" || base == "~" || base == "write" || base == "putc" || base == "param" || base == "ret" || base == "jz" || base == "jnz" || base == "free" || base == "drop" || base == "alloc" || base == "load" || base == "callptr") return { 0, unused, unused };
    if (writesTo(base) >= 0 && base != "read" && base != "getc") return { 0, 1, unused };
    if (base == "store" || base == "check" || base == "checkptr") return { 0, 1, unused };
    return { unused, unused, unused };
}
/**
 * @brief 识别可消除复制的无声明编译器临时量。
 * @param name 名字。
 * @return 是否 t 加数字。
 */
bool optimizationTemporary(const Memory::String &name)
{
    if (name.size() < 2 || name[0] != 't') return false;
    for (std::size_t i = 1; i < name.size(); ++i)
        if (name[i] < '0' || name[i] > '9') return false;
    return true;
}
/**
 * @brief 基本块内传播同类型复制与消除同类型转换，不跨过调用或控制流边界。
 * @param code 四元式。
 * @param declared 声明类型。
 * @param report 优化统计。
 * @return 是否改变。
 */
bool optimizeLocalCopies(Memory::Vector<Quad> &code, const Memory::Map<Memory::String, TypeInfo> &declared,
                         OptimizationReport &report)
{
    bool changed = false;
    Memory::Map<Memory::String, Memory::String> copies;
    Memory::Map<Memory::String, TypeInfo> values;
    const auto same = [](const TypeInfo &a, const TypeInfo &b) {
        return a.bits == b.bits && a.isFloat == b.isFloat && a.isUnsigned == b.isUnsigned;
    };
    const auto lookup = [&](const Memory::String &name, TypeInfo &out) {
        const auto known = values.find(name);
        if (known != values.end())
        {
            out = known->second;
            return true;
        }
        const auto type = declared.find(name);
        if (type != declared.end())
        {
            out = type->second;
            return true;
        }
        return false;
    };
    for (auto &quad : code)
    {
        Memory::String suffix;
        auto base = splitType(quad.op, suffix);
        for (const auto index : optimizationReads(base))
        {
            if (index >= quad.args.size()) continue;
            if (base == "write" && suffix.empty()) continue;
            const auto found = copies.find(quad.arg(index));
            if (found != copies.end())
            {
                quad.args[index] = found->second;
                ++report.copiesPropagated;
                changed = true;
            }
        }
        TypeInfo source;
        const bool known = lookup(quad.arg(0), source);
        if (base == "cvt" && known && same(source, typeOf(suffix)))
        {
            quad.op = ":=";
            base = ":=";
            ++report.conversionsRemoved;
            changed = true;
        }
        const int written = writesTo(base);
        if (written >= 0)
        {
            const auto destination = quad.arg(static_cast<std::size_t>(written));
            for (auto it = copies.begin(); it != copies.end();)
            {
                if (it->first == destination || it->second == destination)
                    it = copies.erase(it);
                else
                    ++it;
            }
            values.erase(destination);
            TypeInfo result;
            bool resultKnown = false;
            if (base == ":=" && known)
            {
                result = source;
                resultKnown = true;
            }
            else if (base == "cvt" || base == "+" || base == "-" || base == "*" || base == "/" || base == "%" || base == "neg")
            {
                result = typeOf(suffix);
                resultKnown = base == "cvt" || base == "neg";
                if (!resultKnown)
                {
                    TypeInfo right;
                    if (known && lookup(quad.arg(1), right))
                    {
                        result.isFloat = source.isFloat || right.isFloat;
                        resultKnown = true;
                    }
                    else if (known && !source.isFloat && !result.isFloat)
                    {
                        bool number = false, floating = false;
                        std::int64_t integer = 0;
                        double real = 0;
                        Memory::String text;
                        resultKnown = readLiteral(quad.arg(1), number, floating, integer, real, text) && number && !floating;
                    }
                }
            }
            else if (base == "==" || base == "!=" || base == "<" || base == "<=" || base == ">" || base == ">=")
            {
                result = typeOf("i64");
                resultKnown = true;
            }
            const auto target = declared.find(destination);
            if (target != declared.end())
            {
                result = target->second;
                resultKnown = true;
            }
            if (resultKnown) values[destination] = result;
            // 原复制保留到死临时量分析，避免提前删除可能报告未定义源的读取。
            if (base == ":=" && known && resultKnown && same(source, result) && destination != quad.arg(0))
                copies[destination] = quad.arg(0);
        }
        const bool pure = written >= 0 && base != "read" && base != "getc" && base != "mul" && base != "div";
        if (!pure && base != "param" && base != "write" && base != "putc")
        {
            copies.clear();
            values.clear();
        }
    }
    Memory::Map<Memory::String, std::size_t> reads;
    for (const auto &quad : code)
        for (const auto &name : readOperands(quad)) ++reads[name];
    std::size_t output = 0;
    for (std::size_t i = 0; i < code.size(); ++i)
    {
        Memory::String suffix;
        const auto base = splitType(code[i].op, suffix);
        const auto temporary = code[i].arg(2);
        const bool pure = base == "+" || base == "-" || base == "*" || base == "/" || base == "%" || base == "cvt" || base == "neg" || base == "~" || base == "==" || base == "!=" || base == "<" || base == "<=" || base == ">" || base == ">=";
        if (i + 1 < code.size() && pure && optimizationTemporary(temporary) && !declared.count(temporary) && reads[temporary] == 1 && code[i + 1].op == ":=" && code[i + 1].arg(0) == temporary && !code[i + 1].arg(2).empty())
        {
            code[i].args[2] = code[i + 1].arg(2);
            ++i;
            if (output != i - 1) code[output] = std::move(code[i - 1]);
            ++output;
            ++report.copiesPropagated;
            changed = true;
            continue;
        }
        if (output != i) code[output] = std::move(code[i]);
        ++output;
    }
    code.resize(output);
    return changed;
}

}  // namespace runtimeDetail
}  // namespace stackvm
