/**
 * @file backend_ownership_test.cpp
 * @brief 目标存储合并、重复检查与微型内联的生成证据及回退测试。
 */
#include "ast/aggregate_optimize.hpp"
#include "stackvm/machine_optimize.hpp"
#include "chali_aot.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
/**
 * @brief 在独立单测进程内配置优化开关。
 * @param name 开关名称。
 * @param value 开关值；空字符串表示关闭该环境设置。
 */
void environment(const char *name, const char *value)
{
#ifdef _WIN32
    _putenv_s(name, value);
#else
    if (*value)
        setenv(name, value, 1);
    else
        unsetenv(name);
#endif
}

/**
 * @brief 失败时报告具体生成约束。
 * @param condition 必须成立的条件。
 * @param text 失败原因。
 */
void require(bool condition, const char *text)
{
    if (!condition) throw std::runtime_error(text);
}

/**
 * @brief 独立验证重复检查的屏障，避免其他优化改变测试输入。
 * @param middle 两个相同检查之间的指令。
 * @param expected 应删除的检查数量。
 */
void checks(const Memory::String &middle, std::size_t expected)
{
    const auto source = stackvm::parseQuads("(check, i, n, items)\n" + middle + "(check, i, n, items)\n", "checks");
    stackvm::machine::Stats stats;
    stackvm::machine::optimize(source, &stats);
    require(stats.redundantChecks == expected, "检查事实未在正确的屏障失效");
}

/**
 * @brief 构造降低器新目标标记，验证普通初始化与自引用初始化分流。
 * @param alias 是否把新目标作为参数传给工厂。
 * @return 用于检查内部目标变体的程序。
 */
ast::Program program(bool alias)
{
    using namespace ast;
    auto point = Type::structOf("Point", { { "x", Type::i64() } });
    Function make;
    make.name = "make";
    make.result = point;
    if (alias) make.parameters.emplace_back("source", Type::i64());
    make.body = Node::block({ Node::declare("__chali_ret_v$0", point),
                              Node::assign("__chali_ret_v$0", Node::structNew("Point", { Node::i(0) })),
                              Node::storeAt(Node::var("__chali_ret_v$0"), Node::i(42), Type::i64()),
                              Node::ret(Node::var("__chali_ret_v$0")) });
    const auto call = alias ? Node::call("make", { "source" }, { Node::var("target") }) : Node::call("make", {}, {});
    const auto move = Node::block({ Node::declare("move_source", Type::i64()), Node::assign("move_source", call),
                                    Node::block({ Node::exprStmt(Node::call("__chali_copy_struct", { "to", "from", "slots" },
                                                                            { Node::var("target"), Node::var("move_source"), Node::i(1) })) }) });
    move->typeName = "__fresh_aggregate_destination";
    Function main;
    main.name = "main";
    main.body = Node::block({ Node::declare("target", point), Node::assign("target", Node::alloc(Node::i(8))), move, Node::ret(Node::i(0)) });
    Program out;
    out.types = { point };
    out.functions = { make, main };
    return out;
}

/**
 * @brief 计数 LLVM 中指定调用形态，用于核对地址验证消除。
 * @param text 完整 LLVM 模块。
 * @param needle 要计数的调用文本。
 * @return 出现次数。
 */
std::size_t occurrences(const Memory::String &text, const Memory::String &needle)
{
    std::size_t count = 0, at = 0;
    while ((at = text.find(needle, at)) != Memory::String::npos)
    {
        ++count;
        at += needle.size();
    }
    return count;
}

/**
 * @brief 编译可执行的槽复制边界用例，核对快速路径和逐槽回退。
 * @param prefix 临时产物前缀。
 * @param mode overlap、self、empty、stale 或 partial。
 */
void buildCopyFixture(const Memory::String &prefix, const Memory::String &mode)
{
    Memory::String body = "(alloc, 32, _, p)\n(store:i64, p, 1, _)\n(+:i64, p, 8, p1)\n(store:i64, p1, 2, _)\n"
                          "(+:i64, p, 16, p2)\n(store:i64, p2, 3, _)\n(+:i64, p, 24, p3)\n(store:i64, p3, 4, _)\n";
    Memory::String target = "p1", source = "p", slots = "3", result = "p3";
    if (mode == "self")
    {
        target = "p";
        result = "p2";
    }
    if (mode == "empty")
    {
        target = "0";
        source = "0";
        slots = "0";
        result = "p";
    }
    if (mode == "stale")
    {
        body += "(alloc, 32, _, q)\n(drop, p, _, _)\n";
        target = "q";
        slots = "1";
    }
    if (mode == "partial")
    {
        body += "(alloc, 8, _, q)\n";
        target = "q";
        slots = "2";
    }
    body += "(param, " + target + ", _, d)\n(param, " + source + ", _, s)\n(param, " + slots + ", _, n)\n(call, __chali_copy_struct, _, ignored)\n";
    body += "(load:i64, " + result + ", _, value)\n(+:i64, value, 48, character)\n(param, character, _, c)\n(call, putchar, _, ignored)\n(ret, 0, _, _)\n";
    const auto quads = stackvm::parseQuads("(call, main, _, _)\n(halt, _, _, _)\n(label, _, _, main)\n" + body + "(decl, i64, local, d)\n(decl, i64, local, s)\n(decl, i64, local, n)\n(label, _, _, __chali_copy_struct)\n"
                                                                                                                 "(decl, i64, local, k)\n(:=, 0, _, k)\n(label, _, _, loop)\n(<:i64, k, n, more)\n(jz, more, _, done)\n"
                                                                                                                 "(*:i64, k, 8, offset)\n(+:i64, s, offset, from)\n(load:i64, from, _, word)\n(+:i64, d, offset, to)\n(store:i64, to, word, _)\n"
                                                                                                                 "(+:i64, k, 1, k)\n(j, _, _, loop)\n(label, _, _, done)\n(ret, 0, _, _)\n",
                                           "copy-boundary");
    chali_aot::Request request;
    request.quads = &quads;
    request.options.outputPrefix = prefix;
    request.options.memoryBytes = 1024 * 1024;
    request.dataBytes = Memory::String(1, '\0');
    request.dataLayout = "zero offset=0 size=1 stride=1\n";
    chali_aot::ExternFunction output;
    output.label = "putchar";
    output.symbol = "putchar";
    output.resultType = "int";
    output.parameterTypes = { "int" };
    request.externs.push_back(output);
    Memory::Vector<Memory::String> diagnostics;
    const int status = chali_aot::run(request, &diagnostics);
    for (const auto &message : diagnostics) std::cerr << message << '\n';
    require(status == 0, "复制边界 AOT 编译失败");
}
}  // namespace

/**
 * @brief 执行低层生成证据检查；不把性能阈值作为语义断言。
 * @return 全部通过为零，失败输出原因并返回一。
 */
int main(int argc, char **argv)
{
    try
    {
        if (argc == 4 && Memory::String(argv[1]) == "--copy-fixture")
        {
            buildCopyFixture(argv[2], argv[3]);
            return 0;
        }
        for (const auto *name : { "CHALI_NO_OPT_INLINE", "CHALI_NO_OPT_STRENGTH", "CHALI_NO_OPT_FOLD", "CHALI_NO_OPT_GVN", "CHALI_NO_OPT_DCE", "CHALI_NO_OPT_LICM" }) environment(name, "1");
        environment("CHALI_NO_MACHINE_OPT", "");
        environment("CHALI_NO_OPT_CHECKS", "");
        checks("", 1);
        checks("(:=, 7, _, i)\n", 0);
        checks("(call, unknown, _, result)\n", 0);
        checks("(label, _, _, join)\n", 0);
        checks("(store:i64, p, 1, _)\n", 0);
        checks("(drop, p, _, _)\n", 0);
        checks("(alloc, 8, _, p)\n", 0);
        environment("CHALI_NO_OPT_INLINE", "");
        stackvm::machine::Stats inlineStats;
        const auto tiny = stackvm::parseQuads("(param, 41, _, x)\n(call, tiny, _, answer)\n(write, answer, _, _)\n(halt, _, _, _)\n"
                                              "(decl, i64, local, x)\n(label, _, _, tiny)\n(+:i64, x, 1, result)\n(ret, result, _, _)\n",
                                              "tiny");
        stackvm::machine::optimize(tiny, &inlineStats);
        require(inlineStats.inlined == 0, "VM 非循环函数被展开，破坏断点和调用栈身份");
        environment("CHALI_NO_AGGREGATE_OPT", "");
        require(ast::optimizeAggregateDestinations(program(false)).functions.size() == 3, "新存储未生成目标地址变体");
        require(ast::optimizeAggregateDestinations(program(true)).functions.size() == 2, "自引用初始化错误合并目标地址");
        environment("CHALI_NO_AGGREGATE_OPT", "1");
        require(ast::optimizeAggregateDestinations(program(false)).functions.size() == 2, "目标地址回退开关无效");
        environment("CHALI_NO_AGGREGATE_OPT", "");
        const auto quads = stackvm::parseQuads("(call, main, _, _)\n(halt, _, _, _)\n(label, _, _, main)\n"
                                               "(alloc, 8, _, p)\n(store:i64, p, 7, _)\n(load:i64, p, _, first)\n(load:i64, p, _, second)\n(drop, p, _, _)\n(ret, second, _, _)\n",
                                               "address-cache");
        chali_aot::Request request;
        request.quads = &quads;
        Memory::Vector<Memory::String> diagnostics;
        environment("CHALI_NO_AOT_ADDRESS_CACHE", "1");
        const auto plain = chali_aot::generateLLVM(request, &diagnostics);
        for (const auto &message : diagnostics) std::cerr << message << '\n';
        require(diagnostics.empty() && !plain.empty(), "原始 LLVM 生成失败");
        environment("CHALI_NO_AOT_ADDRESS_CACHE", "");
        const auto optimized = chali_aot::generateLLVM(request, &diagnostics);
        require(diagnostics.empty() && !optimized.empty(), "优化 LLVM 生成失败");
        require(occurrences(plain, "call ptr @chali_host_address(") == occurrences(optimized, "call ptr @chali_host_address(") + 1, "重复地址验证未按证明消除");
        require(optimized.find("inlinehint") != Memory::String::npos, "极小 AOT 函数缺少内联提示");
        const auto callbacks = stackvm::parseQuads("(call, main, _, _)\n(halt, _, _, _)\n(label, _, _, main)\n"
                                                   "(addr, function_label, _, fp)\n(param, 41, _, x)\n(callptr, fp, _, answer)\n(ret, answer, _, _)\n"
                                                   "(decl, i64, local, x)\n(label, _, _, tiny)\n(+:i64, x, 1, result)\n(ret, result, _, _)\n",
                                                   "known-function");
        request.quads = &callbacks;
        request.dataBytes = Memory::String("\0tiny\0", 6);
        request.dataLayout = "function_label offset=1 size=5 stride=1\n";
        environment("CHALI_NO_AOT_DIRECT_CALLS", "1");
        const auto indirect = chali_aot::generateLLVM(request, &diagnostics);
        environment("CHALI_NO_AOT_DIRECT_CALLS", "");
        const auto direct = chali_aot::generateLLVM(request, &diagnostics);
        require(diagnostics.empty(), "函数值调用 LLVM 生成失败");
        require(occurrences(indirect, "call i64 @chali_invoke(") == occurrences(direct, "call i64 @chali_invoke(") + 1,
                "已证明的无环境函数值未降低为直接调用");
        std::cout << "backend ownership generation OK\n";
        return 0;
    } catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
