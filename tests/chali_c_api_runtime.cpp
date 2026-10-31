/** @file chali_c_api_runtime.cpp @brief C 服务的缓冲、记录、错误及借用调用器回归。 */
#include "abi/chali_abi_runtime.hpp"
#include "abi/chali_abi_internal.h"
#include "abi/chali_abi_registry.hpp"
#include "ircode/runtime_internal.hpp"
#include "c_api_failure_arena.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

using namespace stackvm;
using namespace stackvm::runtimeDetail;

/** @brief 用可变状态和复制计数检查调用器按引用复用。 */
struct Counter
{
    int *copies;
    int value = 0;
    /** @brief 关联复制计数。
     * @param count 外部计数器。
     */
    explicit Counter(int *count) : copies(count) {}
    /** @brief 记录捕获副本。
     * @param other 原对象。
     */
    Counter(const Counter &other) : copies(other.copies), value(other.value) { ++*copies; }
    /** @brief 转移捕获。
     * @param other 原对象。
     */
    Counter(Counter &&other) = default;
    /**
     * @brief 推进捕获状态、返回浮点结果或抛出受控异常。
     * @param function 测试的受检查调用身份。
     * @param arguments 经过 C 服务解码的实参。
     * @param result 返回槽。
     * @return 已处理时为真。
     */
    bool operator()(std::int64_t function, const Memory::Vector<NativeValue> &arguments, NativeValue &result)
    {
        if (function == 1)
        {
            result = NativeValue::fromInt(++value);
            return true;
        }
        if (function == 2)
        {
            assert(arguments.size() == 1 && arguments[0].isFloat());
            result = NativeValue::fromFloat(arguments[0].asFloat() + 0.5);
            return true;
        }
        if (function == 3) throw std::runtime_error("callback service failure");
        if (function == 4) throw 17;
        return false;
    }
};

static int destroyed = 0;
/**
 * @brief 销毁宿主整数记录并记录次数。
 * @param value 拥有的宿主对象。
 */
void destroyInteger(void *value)
{
    ++destroyed;
    delete static_cast<int *>(value);
}
/**
 * @brief 创建独立记录副本。
 * @param value 借用的原记录。
 * @return 独立副本。
 */
void *cloneInteger(const void *value) { return new int(*static_cast<const int *>(value)); }
/**
 * @brief 用另一身份的销毁函数检查类型冲突。
 * @param value 未转移所有权的对象。
 */
void conflictingDestroy(void *value) { delete static_cast<int *>(value); }

/**
 * @brief 验证完整 C 服务路径，所有新建资源均显式释放。
 * @return 全部断言通过时为零。
 */
int main()
{
    FailingArena arena(8 * 1024 * 1024);
    {
        Memory::AllocationScope allocation(&arena);
        DataArea data;
        data.binary.assign({ 0, 't', 'e', 'x', 't', 0 });
        Heap heap;
        heap.reset(64);
        ObjectStore objects;
        NativeContext context;
        context.heap = &heap;
        context.data = &data;
        context.objects = &objects;
        int copies = 0;
        NativeInvoker invoker{ Counter(&copies) };
        context.invoker = &invoker;
        objects.recordContext = context;
        ChaliAbiRuntime runtime{ &context };
        ChaliAbiFrame frame{ &runtime, nullptr, nullptr, 0, 0, 0 };
        const auto reset = [&] { context.error.clear(); context.memoryFault.clear(); runtime.status = 0; runtime.diagnostic.clear(); runtime.fallback = nullptr; };
        const unsigned char *view = nullptr;
        std::size_t length = 0;
        assert(chali_abi_bytes(&frame, 1, &view, &length) && std::string(reinterpret_cast<const char *>(view), length) == "text");
        const auto parseError = chali_abi_resolve("std::parse::__parseFloat64Error", nullptr);
        const auto parseValue = chali_abi_resolve("std::parse::__parseFloat64Value", nullptr);
        assert(parseError && parseValue);
        for (const auto &text : { std::string(), std::string("3.5"), std::string("1e309x"), std::string("nan"), std::string(300, '0') + ".5" })
        {
            std::int64_t handle = chali_abi_string(&frame, text.data(), text.size());
            unsigned char kind = ChaliAbiSlotInteger;
            ChaliAbiFrame parsed{ &runtime, &handle, &kind, 1, 0, 0 };
            parseError(&parsed);
            const auto expected = text.empty() ? 1 : text == "1e309x" || text == "nan" ? 2 : 0;
            assert(runtime.status == 0 && parsed.result == expected && !parsed.resultIsFloat);
            if (expected == 0)
            {
                parseValue(&parsed);
                double number = 0.0;
                std::memcpy(&number, &parsed.result, sizeof(number));
                assert(runtime.status == 0 && parsed.resultIsFloat && number == (text == "3.5" ? 3.5 : 0.5));
            }
            assert(chali_abi_string_release(&frame, handle));
            parseError(&parsed);
            assert(runtime.status != 0);
            reset();
            kind = ChaliAbiSlotFloat;
            parseError(&parsed);
            assert(runtime.status != 0);
            reset();
        }
        std::puts("C float parser: empty, buffered, overflow priority, stale handles and slot kinds passed");
        for (const auto &text : { std::string(), std::string("中文"), std::string(300, 'x') })
        {
            const auto handle = chali_abi_string(&frame, text.data(), text.size());
            assert(handle && runtime.status == 0);
            assert(chali_abi_bytes(&frame, handle, &view, &length) && length == text.size());
            if (length) assert(std::memcmp(view, text.data(), length) == 0);
            assert(chali_abi_string_release(&frame, handle));
            assert(!chali_abi_bytes(&frame, handle, &view, &length) && runtime.status != 0);
            reset();
        }
        assert(!chali_abi_string(&frame, "a\0b", 3) && runtime.status != 0);
        reset();
        assert(!chali_abi_string(&frame, nullptr, 1) && runtime.status != 0);
        reset();
        assert(!chali_abi_bytes(&frame, -1, &view, &length) && runtime.status != 0);
        reset();
        char byte = 0;
        assert(!chali_abi_read(&frame, 0, &byte, 1) && runtime.status != 0);
        reset();
        const auto temporary = chali_abi_temporary(&frame, 16);
        assert(temporary);
        const std::int64_t original[2] = { 7, 11 };
        std::int64_t restored[2]{};
        assert(chali_abi_write(&frame, temporary, original, sizeof(original)));
        assert(chali_abi_read(&frame, temporary, restored, sizeof(restored)) && restored[0] == 7 && restored[1] == 11);
        assert(!chali_abi_read(&frame, temporary + 8, restored, sizeof(restored)) && runtime.status != 0);
        reset();
        assert(chali_abi_temporary_release(&frame, temporary));
        assert(!chali_abi_temporary_release(&frame, temporary) && runtime.status != 0);
        reset();
        std::int64_t result = 0;
        int floating = 0;
        const auto initialCopies = copies;
        assert(chali_abi_invoke(&frame, 1, nullptr, nullptr, 0, &result, &floating) && result == 1 && !floating);
        assert(chali_abi_invoke(&frame, 1, nullptr, nullptr, 0, &result, &floating) && result == 2 && copies == initialCopies);
        NativeRegistry legacy;
        ChaliAbiRegistry borrowed{ {}, &legacy };
        assert(legacy.add("through_context", 0, [](NativeCallContext &host, const auto &arguments) {
            NativeValue returned;
            assert(host.invoke(1, arguments, returned));
            return returned;
        }));
        char diagnostic[128];
        ChaliAbiError error{ diagnostic, sizeof(diagnostic), 0, 0 };
        assert(chali_abi_registry_call(&borrowed, "through_context", &frame, &error) == ChaliAbiOk && frame.result == 3);
        assert(copies == initialCopies);
        std::int64_t bits = chali_abi_slot_from_real(1.25);
        unsigned char kind = ChaliAbiSlotFloat;
        assert(chali_abi_invoke(&frame, 2, &bits, &kind, 1, &result, &floating) && floating);
        double real = 0;
        std::memcpy(&real, &result, sizeof(real));
        assert(real == 1.75);
        assert(!chali_abi_invoke(&frame, 1, nullptr, nullptr, 1, &result, &floating) && runtime.status != 0);
        reset();
        assert(!chali_abi_invoke(&frame, 1, &bits, &kind, -1, &result, &floating) && runtime.status != 0);
        reset();
        kind = 2;
        assert(!chali_abi_invoke(&frame, 1, &bits, &kind, 1, &result, &floating) && runtime.status != 0);
        reset();
        assert(!chali_abi_invoke(&frame, 0, nullptr, nullptr, 0, &result, &floating) && runtime.status != 0);
        reset();
        assert(!chali_abi_invoke(&frame, 3, nullptr, nullptr, 0, &result, &floating) && runtime.diagnostic == "callback service failure");
        reset();
        assert(!chali_abi_invoke(&frame, 4, nullptr, nullptr, 0, &result, &floating) && runtime.status != 0);
        reset();
        frame.slots = &bits;
        frame.kinds = &kind;
        frame.count = 1;
        kind = ChaliAbiSlotFloat;
        assert(chali_abi_integer(&frame, 0) == 0 && runtime.status != 0);
        reset();
        kind = ChaliAbiSlotInteger;
        assert(chali_abi_real(&frame, 0) == 0 && runtime.status != 0);
        reset();
        frame.slots = nullptr;
        frame.kinds = nullptr;
        frame.count = 0;
        assert(!chali_abi_read(&frame, 0, &byte, 1));
        chali_abi_fail(&frame, "module detail");
        assert(runtime.diagnostic == "module detail");
        chali_abi_fail(&frame, "later detail");
        assert(runtime.diagnostic == "module detail");
        reset();
        context.error = "prior runtime failure";
        chali_abi_fail(&frame, "module detail");
        assert(runtime.diagnostic == "prior runtime failure");
        reset();
        context.memoryFault = "prior memory failure";
        chali_abi_fail(&frame, "module detail");
        assert(runtime.diagnostic == "prior memory failure");
        reset();
        const auto record = chali_abi_record_make(&frame, "c-api-test::Integer", new int(42), destroyInteger);
        assert(record && *static_cast<int *>(chali_abi_record_borrow(&frame, record, "c-api-test::Integer")) == 42);
        int cloneAttempts = 0;
        for (int point = 0; point < 32; ++point)
        {
            const auto before = destroyed;
            arena.remaining = point;
            const auto attempted = chali_abi_record_clone(&frame, record, "c-api-test::Integer", cloneInteger);
            arena.remaining = -1;
            ++cloneAttempts;
            if (attempted)
            {
                assert(chali_abi_record_release(&frame, attempted) && destroyed == before + 1);
                break;
            }
            assert(runtime.status != 0 && destroyed == before + 1);
            reset();
            assert(*static_cast<int *>(chali_abi_record_borrow(&frame, record, "c-api-test::Integer")) == 42);
            assert(point != 31);
        }
        int creationFailures = 0;
        for (int point = 0; point < 32; ++point)
        {
            int *raw = new int(9);
            const auto before = destroyed;
            arena.remaining = point;
            const auto attempted = chali_abi_record_make(&frame, "c-api-test::Integer", raw, destroyInteger);
            arena.remaining = -1;
            if (attempted)
            {
                assert(chali_abi_record_release(&frame, attempted) && destroyed == before + 1);
                break;
            }
            ++creationFailures;
            assert(runtime.status != 0 && destroyed == before);
            delete raw;
            reset();
            assert(point != 31);
        }
        assert(cloneAttempts > 2 && creationFailures > 1);
        std::printf("record ownership: %d clone attempts and %d creation failure points passed\n", cloneAttempts, creationFailures);
        const auto copy = chali_abi_record_clone(&frame, record, "c-api-test::Integer", cloneInteger);
        assert(copy && copy != record);
        *static_cast<int *>(chali_abi_record_borrow(&frame, copy, "c-api-test::Integer")) = 7;
        assert(*static_cast<int *>(chali_abi_record_borrow(&frame, record, "c-api-test::Integer")) == 42);
        int *conflicting = new int(9);
        assert(!chali_abi_record_make(&frame, "c-api-test::Integer", conflicting, conflictingDestroy) && runtime.status != 0);
        delete conflicting;
        reset();
        assert(!chali_abi_record_borrow(&frame, record, "different::Type") && runtime.status != 0);
        reset();
        assert(chali_abi_record_release(&frame, copy) && chali_abi_record_release(&frame, record) && destroyed == cloneAttempts + 3);
        assert(!chali_abi_record_release(&frame, record) && runtime.status != 0);
        reset();
    }
    assert(arena.stats().liveBlocks == 0);
    std::puts("C services: buffered strings, memory ranges, borrowed invoker, errors and records passed");
}
