/** @file chali_c_api_registry.cpp @brief 注册事务、异常隔离、生命周期与并发查询。 */
#include "abi/chali_abi_registry.hpp"
#include "abi/chali_standard_internal.h"
#include "abi/std_aot_modules.hpp"
#include "abi/chali_abi_runtime.hpp"
#include "c_api_failure_arena.hpp"
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <thread>
#include <vector>

/** @brief 仅供可变回调状态检查使用的上下文。 */
struct UnusedContext final : runtime::NativeCallContext
{
    /** @brief 本测试不提供内存读取。
     * @param address 地址。
     * @param out 输出。
     * @param count 数量。
     * @return 恒为假。
     */
    bool readBytes(std::int64_t address, char *out, std::size_t count) const override
    {
        (void)address;
        (void)out;
        (void)count;
        return false;
    }
    /** @brief 本测试不提供内存写入。
     * @param address 地址。
     * @param data 输入。
     * @param count 数量。
     * @return 恒为假。
     */
    bool writeBytes(std::int64_t address, const char *data, std::size_t count) override
    {
        (void)address;
        (void)data;
        (void)count;
        return false;
    }
    /** @brief 本测试不提供文本读取。
     * @param address 地址。
     * @param out 输出。
     * @return 恒为假。
     */
    bool readString(std::int64_t address, Memory::String &out) const override
    {
        (void)address;
        (void)out;
        return false;
    }
    /** @brief 本测试不调用语言函数。
     * @param function 函数句柄。
     * @param arguments 实参。
     * @param result 输出。
     * @return 恒为假。
     */
    bool invoke(std::int64_t function, const Memory::Vector<runtime::NativeValue> &arguments, runtime::NativeValue &result) override
    {
        (void)function;
        (void)arguments;
        (void)result;
        return false;
    }
    /** @brief 本测试不创建文本。
     * @param text 文本。
     * @return 零句柄。
     */
    std::int64_t makeString(const Memory::String &text) override
    {
        (void)text;
        return 0;
    }
    /** @brief 忽略未使用上下文的诊断。
     * @param text 诊断。
     */
    void fail(const Memory::String &text) override { (void)text; }
};

/**
 * @brief 返回整数一的无状态 C 入口。
 * @param frame 本次调用帧。
 */
void CHALI_ABI_CALL one(ChaliAbiFrame *frame) { chali_abi_return_integer(frame, 1); }

/** @brief 穷举所有模块登记分配失败点，失败后立即重试，排除残留紧凑键。 */
void transactions()
{
    const auto abi = chali_abi_local_info(0);
    const ChaliAbiRecord records[] = { { "transaction::long_first_name", one, 0 }, { "transaction::long_second_name", one, 0 }, { "transaction::long_third_name", one, 0 } };
    const ChaliAbiModuleDescriptor module{ sizeof(module), &abi, records, 3 };
    int failures = 0;
    for (int point = 0; point < 128; ++point)
    {
        FailingArena arena;
        bool complete = false;
        {
            Memory::AllocationScope allocation(&arena);
            runtime::NativeRegistry backend;
            ChaliAbiRegistry registry{ {}, &backend };
            assert(backend.add("existing", 0, [value = 0](auto &, const auto &) mutable { return runtime::NativeValue::fromInt(++value); }));
            const auto *original = backend.find("existing");
            UnusedContext context;
            assert(original->callback(context, {}).asInt() == 1);
            const auto revision = backend.revision();
            char message[256];
            ChaliAbiError error{ message, sizeof(message), 0, 0 };
            arena.remaining = point;
            const auto status = chali_abi_registry_register(&registry, &module, &error);
            arena.remaining = -1;
            if (status != ChaliAbiOk)
            {
                ++failures;
                assert(status == ChaliAbiHostFailure && backend.revision() == revision);
                for (const auto &record : records) assert(!backend.find(record.name));
                assert(chali_abi_registry_register(&registry, &module, &error) == ChaliAbiOk);
            }
            else
                complete = true;
            assert(backend.revision() == revision + 3);
            assert(backend.find("existing") == original);
            assert(original->callback(context, {}).asInt() == 2);
            assert(chali_abi_registry_seal(&registry, &error) == ChaliAbiOk);
            assert(!backend.remove("existing") && !backend.add("later", 0, original->callback));
        }
        assert(arena.stats().liveBlocks == 0);
        if (complete) break;
        assert(point != 127);
    }
    assert(failures > 3);
    std::printf("atomic registration: %d allocation failure points passed\n", failures);
}

/**
 * @brief 抛出标准异常，检验 C 调用边界。
 * @param frame 未使用的帧。
 */
void CHALI_ABI_CALL throwing(ChaliAbiFrame *frame)
{
    (void)frame;
    throw std::runtime_error("entry exception");
}
/**
 * @brief 先报告错误再抛出其他异常，首条错误必须保留。
 * @param frame 调用帧。
 */
void CHALI_ABI_CALL firstFailure(ChaliAbiFrame *frame)
{
    chali_abi_fail(frame, "first failure");
    throw 7;
}
/**
 * @brief 故意破坏输入字段，边界须恢复输入并拒绝结果。
 * @param frame 调用帧。
 */
void CHALI_ABI_CALL clobber(ChaliAbiFrame *frame)
{
    frame->runtime = nullptr;
    frame->slots = nullptr;
    frame->count = -1;
}

/** @brief 检验 C 句柄不借用临时 arena，并发调用不共享临时帧。 */
void boundaries()
{
    char message[256];
    ChaliAbiError error{ message, sizeof(message), 0, 0 };
    ChaliAbiRegistry *registry;
    const auto abi = chali_abi_local_info(0);
    const ChaliAbiRecord records[] = { { "one", one, 0 }, { "throw", throwing, 0 }, { "first", firstFailure, 0 }, { "clobber", clobber, 0 } };
    const ChaliAbiModuleDescriptor module{ sizeof(module), &abi, records, 4 };
    {
        Memory::SystemRuntimeArena temporary(1024 * 1024);
        Memory::AllocationScope allocation(&temporary);
        registry = chali_abi_registry_create(&error);
        assert(registry && chali_abi_registry_register(registry, &module, &error) == ChaliAbiOk);
        assert(temporary.stats().liveBlocks == 0);
    }
    ChaliAbiFrame frame{};
    assert(chali_abi_registry_call(registry, "throw", &frame, &error) == ChaliAbiHostFailure);
    assert(std::string(message) == "entry exception" && !frame.runtime && !frame.result);
    assert(chali_abi_registry_call(registry, "first", &frame, &error) == ChaliAbiHostFailure);
    assert(std::string(message) == "first failure" && !frame.runtime);
    assert(chali_abi_registry_call(registry, "clobber", &frame, &error) == ChaliAbiHostFailure);
    assert(frame.count == 0 && !frame.runtime && !frame.slots);
    assert(chali_abi_registry_seal(registry, &error) == ChaliAbiOk);
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) workers.emplace_back([registry] {
        for (int n = 0; n < 128; ++n)
        {
            ChaliAbiFrame frame{};
            assert(chali_abi_registry_call(registry, "one", &frame, nullptr) == ChaliAbiOk && frame.result == 1 && !frame.runtime);
        }
    });
    for (auto &worker : workers) worker.join();
    chali_abi_registry_destroy(registry);
}

/**
 * @brief 验证标准库统一清单、模块隔离与失败后的已登记入口。
 */
void standardModules()
{
    char text[512];
    ChaliAbiError error{text, sizeof(text), 0, ChaliAbiOk};
    auto *registry = chali_abi_registry_create(&error);
    assert(registry);
    assert(chali_abi_register_tables(nullptr, nullptr, 0, &error) == ChaliAbiInvalidArgument);
    const ChaliAbiModule missing[] = {nullptr};
    assert(chali_abi_register_tables(registry, missing, 1, &error) == ChaliAbiInvalidArgument);
    assert(chali_abi_bind_standard(registry, &error) == ChaliAbiOk);
#define CHALI_STANDARD_ONLY
#define CHALI_AOT_MODULE(moduleName, ...) \
    { \
        const ChaliAbiModule modules[] = {__VA_ARGS__}; \
        for (auto module : modules) \
        { \
            size_t count = 0; \
            const auto *records = module(&count); \
            for (size_t index = 0; index < count; ++index) \
            { \
                ChaliAbiEntry entry = nullptr; \
                int32_t arity = 0; \
                assert(chali_abi_registry_resolve(registry, records[index].name, &entry, &arity, &error) == ChaliAbiOk); \
                assert(entry == records[index].entry && arity == records[index].arity); \
            } \
        } \
    }
#include "abi/aot_modules.inc"
#undef CHALI_AOT_MODULE
#undef CHALI_STANDARD_ONLY
    size_t count = 0;
    const auto *demo = abi_demo_records(&count);
    assert(count);
    ChaliAbiEntry entry = nullptr;
    assert(chali_abi_registry_resolve(registry, demo[0].name, &entry, nullptr, &error) == ChaliAbiNotFound);
    assert(chali_abi_registry_seal(registry, &error) == ChaliAbiOk);
    assert(chali_abi_bind_standard(registry, &error) == ChaliAbiSealed);
    assert(chali_abi_register_tables(registry, missing, 1, &error) == ChaliAbiInvalidArgument);
    chali_abi_registry_destroy(registry);
    Memory::Vector<Memory::String> diagnostics;
    const auto *print = abi_print_records(&count);
    auto selected = std_registry::buildModulesFor({print[0].name}, &diagnostics);
    assert(diagnostics.empty() && selected.size() == 1 && selected[0] == "print");
    selected = std_registry::buildModulesFor({demo[0].name}, &diagnostics);
    assert(diagnostics.empty() && selected.size() == 1 && selected[0] == "demo");
    selected = std_registry::buildModulesFor({"__unregistered_native_entry__"}, &diagnostics);
    assert(diagnostics.empty() && selected.empty());
}

/**
 * @brief 运行注册与调用边界回归。
 * @return 全部断言通过时为零。
 */
int main()
{
    std::vector<std::thread> catalog;
    for (int i = 0; i < 4; ++i) catalog.emplace_back([] {
        for (int n = 0; n < 64; ++n)
        {
            std::size_t count = 0;
            const auto *records = chali_abi_records(&count);
            assert(records && count);
            for (std::size_t at = 0; at < count; ++at)
            {
                int arity = 0;
                assert(chali_abi_resolve(records[at].name, &arity) == records[at].entry && arity == records[at].arity);
            }
        }
    });
    for (auto &thread : catalog) thread.join();
    standardModules();
    transactions();
    boundaries();
    std::puts("C registry exception, ownership and concurrent lookup passed");
}
