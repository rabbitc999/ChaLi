/**
 * @file chali_native_invoker_borrow.cpp
 * @brief 核对同步调用器借用不复制闭包，原有拥有式调用仍隔离可变捕获。
 */
#include "ircode/runtime_internal.hpp"
#include "allocator/system_arena.hpp"
#include "glue/std_collection_state.hpp"
#include <cassert>

/**
 * @brief 用复制计数和可变状态区分借用调用与拥有副本。
 */
struct CountedInvoker
{
    int *copies;
    int value = 0;
    /**
     * @brief 记录供测试观察的复制计数。
     * @param count 在调用器之外存活的计数器。
     */
    explicit CountedInvoker(int *count) : copies(count) {}
    /**
     * @brief 创建独立的可变捕获副本并累计计数。
     * @param other 原调用器。
     */
    CountedInvoker(const CountedInvoker &other) : copies(other.copies), value(other.value) { ++*copies; }
    /**
     * @brief 转移调用器状态，不创建副本。
     * @param other 被移动的调用器。
     */
    CountedInvoker(CountedInvoker &&other) = default;
    /**
     * @brief 推进本捕获的状态并返回结果。
     * @param function 非零时表示有效函数。
     * @param arguments 未使用的同步实参。
     * @param result 接收本次状态。
     * @return 函数标记非零时为真。
     */
    bool operator()(std::int64_t function, const Memory::Vector<stackvm::NativeValue> &arguments, stackvm::NativeValue &result)
    {
        (void)arguments;
        result = stackvm::NativeValue::fromInt(++value);
        return function != 0;
    }
};

/**
 * @brief 分别验证借用和拥有方式的复制、状态与错误传播契约。
 * @return 全部检查通过时返回零。
 */
int main()
{
    Memory::SystemRuntimeArena arena(1024 * 1024);
    Memory::AllocationScope allocation(&arena);
    int copies = 0;
    stackvm::runtimeDetail::NativeContext context;
    stackvm::runtimeDetail::NativeInvoker invoker{ CountedInvoker(&copies) };
    const int initialCopies = copies;
    const auto initialAllocations = arena.stats().totalBlocks;
    stackvm::runtimeDetail::NativeCallContextImpl borrowed(context, &invoker);
    assert(copies == initialCopies && arena.stats().totalBlocks == initialAllocations);
    collection_glue::State collection(borrowed);
    assert(arena.stats().totalBlocks == initialAllocations);
    Memory::Vector<stackvm::NativeValue> arguments;
    stackvm::NativeValue result;
    assert(borrowed.invoke(1, arguments, result) && result.asInt() == 1);
    assert(borrowed.invoke(1, arguments, result) && result.asInt() == 2);
    stackvm::runtimeDetail::NativeCallContextImpl owned(context, invoker);
    assert(copies == initialCopies + 1);
    assert(owned.invoke(1, arguments, result) && result.asInt() == 3);
    assert(invoker(1, arguments, result) && result.asInt() == 3);
    assert(!borrowed.invoke(0, arguments, result));
    assert(!context.error.empty());
}
