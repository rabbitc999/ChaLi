/**
 * @file chali_generic_monomorphization_test.cpp
 * @brief 泛型文本替换、类型身份和递归展开的回归。
 */
#include "chali_generic.hpp"
#include <cstdio>
#include <cstdlib>
#include <algorithm>

/**
 * @brief 检查回归条件。
 * @param value 条件。
 * @param message 失败原因。
 * @return 无。
 */
void require(bool value, const char *message)
{
    if (!value)
    {
        std::fprintf(stderr, "%s\n", message);
        std::exit(1);
    }
}

/**
 * @brief 展开独立测试源码。
 * @param text 源码。
 * @param result 结果。
 * @param error 诊断。
 * @return 展开成功返回 true。
 */
bool expand(const Memory::String &text, preprocess::Result &result, Memory::String &error)
{
    result = preprocess::Result{};
    result.text = text;
    error.clear();
    return chali_generic::run(result, error);
}

/**
 * @brief 验证不能由运行结果直接观察的实例复用和文本边界。
 * @return 成功返回零。
 */
int main()
{
    using namespace chali_generic;
    const Memory::String literal = R"(T "T \\" /* T */ // T
'T' TT T)";
    const auto replaced = substituteIdentifiers(literal, { { "T", "int64" } });
    require(replaced == R"(int64 "T \\" /* T */ // T
'T' TT int64)",
            "字符串、字符、注释或标识符边界发生误替换");
    require(mangle("G", { "(int64,std::core::String)" }) != mangle("G", { "_int64_lang__String_" }), "标点不同的类型发生实例名称碰撞");
    require(mangle("G", { Memory::String(4000u, 'a') }).size() < 100u, "实例名称随嵌套类型无限增长");
    preprocess::Result result;
    Memory::String error;
    require(expand("struct AuditUnit<T,V=T,W=AuditUnitInner<T,V>>{W value;};"
                   "struct AuditUnitInner<T,V>{T a;V b;};AuditUnit<String> a;AuditUnit<std::core::String> b;",
                   result, error),
            error.c_str());
    std::size_t outer = 0, inner = 0;
    for (const auto &entry : session.made)
    {
        if (entry.second.generic->name == "AuditUnit")
        {
            ++outer;
            require(entry.second.args[0] == "std::core::String" && entry.second.args[1] == "std::core::String", "依赖默认类型未绑定或归一");
        }
        if (entry.second.generic->name == "AuditUnitInner") ++inner;
    }
    require(outer == 1u && inner == 1u, "重复使用或类型别名生成了重复实例");
    require(!expand("struct AuditForward<T=V,V=int64>{T value;};", result, error) && error.find("只能引用前面的类型参数") != Memory::String::npos, "前向默认类型缺少明确诊断");
    require(!expand("struct AuditCycle<T=AuditCycle<>>{T value;};AuditCycle<> a;", result, error) && error.find("128") != Memory::String::npos, "递归默认类型没有及时终止");
    require(!expand("struct AuditGrow<T>{AuditGrow<List<T>> mut& next;};AuditGrow<int64> a;", result, error) && (error.find("1024") != Memory::String::npos || error.find("128") != Memory::String::npos), "不断扩大的递归类型没有及时终止");
    Memory::String deepDefault = "int64";
    for (std::size_t i = 0; i < 128u; ++i) deepDefault = "List<" + deepDefault + ">";
    require(!expand("struct AuditDeepDefault<T=" + deepDefault + ">{T value;};AuditDeepDefault<> a;", result, error) && error.find("128") != Memory::String::npos, "深层容器默认类型没有及时终止");
    Memory::String many = "struct AuditMany<T>{T value;};";
    for (std::size_t i = 0; i < 1025u; ++i)
        many += "AuditMany<N" + Memory::toString(i) + "> a" + Memory::toString(i) + ";";
    require(!expand(many, result, error) && error.find("1024") != Memory::String::npos, "实例总数上限没有及时终止");
    collectCoreAliasShadows("module demo;struct Concrete{};struct Generic<T>{T value;};");
    require(canonicalType("Concrete") == canonicalType("demo::Concrete"), "导入裸名称与限定名称的类型身份不一致");
    require(canonicalType("demo::Generic__int64") == "Generic__int64", "泛型实例限定名称未归一");
    require(mangle("native", { "int64 fn(int64)", "(int64,std::core::String)" }) == "native____chl_330bfbc9c27b9276_int64_fn_int64_____chl_4e24b30d525edf54__int64_std__core__String_", "复杂类型的冻结宿主符号发生变化");
    require(mangle("native", { "int32" }) == "native__int32", "简单类型的既有宿主符号发生变化");
    Memory::String marked = "List<List<int64>> xs; List<int64 fn(int64)> fs; List</* < > */int64> cs; (a<b); a<b; a>>1;";
    markContainerTypeArguments(marked);
    require(marked == "List@<List@<int64@>@> xs; List@<int64 fn(int64)@> fs; List@</* < > */int64@> cs; (a<b); a<b; a>>1;", "容器标点破坏了比较、右移、注释或函数类型");
    auto once = marked;
    markContainerTypeArguments(marked);
    require(marked == once, "容器标点归一化不幂等");
    Memory::String memberInterfaces = "$implement Count<T>;\n$implement Count<String>;";
    markContainerTypeArguments(memberInterfaces);
    require(memberInterfaces == "$implement Count@<T@>;\n$implement Count@<String@>;", "成员宏接口实参未保留类型语法");
    preprocess::Result mapped;
    mapped.text = "struct AuditOrigin<T>{\nT value;\n};\n";
    for (std::size_t line = 0; line < 3; ++line) mapped.lineMap.push_back({ "mapped.chl", 100u + line });
    require(run(mapped, error), error.c_str());
    Memory::String generated;
    Memory::Vector<preprocess::LineOrigin> origins;
    require(instantiate("AuditOrigin", { "int64" }, generated, error, &origins), error.c_str());
    require(origins.size() == static_cast<std::size_t>(std::count(generated.begin(), generated.end(), '\n')), "实例行映射数量与生成源码不一致");
    bool foundFieldOrigin = false;
    for (const auto &origin : origins)
        if (origin.file == "mapped.chl" && origin.line == 101) foundFieldOrigin = true;
    require(foundFieldOrigin, "语义期实例化丢失了字段原始行");
    const auto projected = substituteIdentifiers("T\n.\nItem value;", { { "T", "Concrete" } });
    require(std::count(projected.begin(), projected.end(), '\n') == 2, "关联投影替换改变了后续声明的行号");
    require(expand("module sql_fixture;struct LegacyStatement{};module helper_fixture;struct LegacyBox<T>{T value;};module program;", result, error), error.c_str());
    generated.clear();
    require(instantiate("helper_fixture::LegacyBox", { "LegacyStatement" }, generated, error), error.c_str());
    bool preservedLegacyArgument = false;
    for (const auto &entry : session.made)
        if (entry.second.generic->sourceName == "LegacyBox" && entry.second.args[0] == "sql_fixture::LegacyStatement") preservedLegacyArgument = true;
    require(preservedLegacyArgument, "无来源的既有合成调用未保留已解析裸名的兼容归一");
    collectCoreAliasShadows("module std::core;opcode String{};module alpha;struct String{};struct OnlyAlpha{};module program;struct String{};module client;import alpha;");
    require(canonicalTypeInModule("String", "std::core", error) == "std::core::String" && error.empty(), "标准库类型实参被用户同名类型捕获");
    require(canonicalTypeInModule("String", "program", error) == "program::String" && error.empty(), "用户类型实参被标准别名捕获");
    require(canonicalTypeInModule("String", "alpha", error) == "alpha::String" && error.empty(), "本模块同名类型未优先绑定");
    require(canonicalTypeInModule("List<String>", "std::core", error) == "std::core::List<std::core::String>" && error.empty(), "嵌套容器丢失模块来源");
    require(canonicalTypeInModule("String const &", "std::core", error) == "std::core::String const&" && error.empty(), "引用包装丢失模块来源");
    require(canonicalTypeInModule("(String,int64)", "alpha", error) == "(alpha::String,int64)" && error.empty(), "元组包装丢失模块来源");
    require(canonicalTypeInModule("String fn(String)", "std::core", error) == "std::core::String fn(std::core::String)" && error.empty(), "函数类型丢失模块来源");
    require(canonicalTypeInModule("OnlyAlpha", "client", error) == "alpha::OnlyAlpha" && error.empty(), "直接导入类型未绑定");
    canonicalTypeInModule("String", "client", error);
    require(error.find("歧义") != Memory::String::npos, "导入同名核心类型未报告歧义");
    canonicalTypeInModule("OnlyAlpha", "unrelated", error);
    require(error.find("声明或导入") != Memory::String::npos, "不可见类型被全局唯一名称泄露");
    require(canonicalTypeInModule("int64", "program", error) == "int64" && error.empty(), "成功调用未清除上一轮错误");

    preprocess::Result switchSource;
    switchSource.text = "#NoEqualReplace\nstruct AuditSwitch<T>{T value;};\n";
    for (std::size_t line = 0; line < 2; ++line)
        switchSource.lineMap.push_back({ "switches.chl", 200u + line });
    require(run(switchSource, error), error.c_str());
    require(session.generics.at("AuditSwitch").operatorSwitches.find("#NoEqualReplace") != Memory::String::npos, "泛型运算符开关未保留");
    generated.clear();
    origins.clear();
    require(instantiate("AuditSwitch", { "int64" }, generated, error, &origins), error.c_str());
    require(generated.find("#NoEqualReplace") != Memory::String::npos, "泛型实例丢失运算符开关");
    std::puts("generic monomorphization unit tests OK");
}
