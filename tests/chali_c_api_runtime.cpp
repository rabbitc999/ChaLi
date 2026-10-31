/** @file chali_c_api_runtime.cpp @brief C 服务的缓冲、记录、错误及借用调用器回归。 */
#include "abi/chali_abi_runtime.hpp"
#include "abi/chali_abi_internal.h"
#include "abi/chali_abi_registry.hpp"
#include "ircode/runtime_internal.hpp"
#include "c_api_failure_arena.hpp"
#include "glue/std_collection.hpp"
#include <cassert>
#include <atomic>
#include <cstdlib>
#include <new>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

static std::atomic<std::size_t> allocationEvents{0};
/**
 * @brief 记录本测试可执行文件的实际 C++ 堆分配，不改变系统分配器。
 * @param size 请求字节数。
 * @return 系统 malloc 分配的块。
 * @throws std::bad_alloc 系统分配失败。
 */
void *operator new(std::size_t size)
{
    allocationEvents.fetch_add(1, std::memory_order_relaxed);
    if (void *value = std::malloc(size ? size : 1)) return value;
    throw std::bad_alloc();
}
/**
 * @brief 数组分配复用同一系统分配及计数。
 * @param size 请求字节数。
 * @return 已分配的块。
 */
void *operator new[](std::size_t size) { return ::operator new(size); }
/**
 * @brief 释放测试中的系统分配。
 * @param value 可为空的分配块。
 */
void operator delete(void *value) noexcept { std::free(value); }
/**
 * @brief 释放数组对应的系统分配。
 * @param value 可为空的分配块。
 */
void operator delete[](void *value) noexcept { std::free(value); }
/**
 * @brief 处理带大小的释放形式。
 * @param value 可为空的分配块。
 * @param size 编译器提供的原大小，不用于系统 free。
 */
void operator delete(void *value, std::size_t size) noexcept { (void)size; std::free(value); }
/**
 * @brief 处理带大小的数组释放形式。
 * @param value 可为空的分配块。
 * @param size 编译器提供的原大小，不用于系统 free。
 */
void operator delete[](void *value, std::size_t size) noexcept { (void)size; std::free(value); }

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
 * @brief 模拟不提供副本的 C 回调。
 * @param value 不读取的源记录。
 * @return 空指针。
 */
void *failedClone(const void *value) { (void)value; return nullptr; }
/**
 * @brief 模拟错误返回源指针的 C 回调，验证运行时不误销毁源。
 * @param value 源记录。
 * @return 原指针。
 */
void *aliasedClone(const void *value) { return const_cast<void *>(value); }
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
        const auto transcode = chali_abi_resolve("std::serialization::__transcode", nullptr);
        assert(transcode && !chali_abi_resolve("std::serialization::__pack", nullptr) &&
               !chali_abi_resolve("std::serialization::__unpack", nullptr));
        for (const auto &text : {std::string("{\"value\":null}"), std::string("[]"), std::string("{"),
                                 std::string("{\"value\":\"中文\"}"), std::string(4096, ' ') + "{}"})
        {
            int64_t arguments[] = {chali_abi_string(&frame, text.data(), text.size()), 1};
            unsigned char kinds[] = {ChaliAbiSlotInteger, ChaliAbiSlotInteger};
            ChaliAbiFrame converted{&runtime, arguments, kinds, 2, 0, 0};
            transcode(&converted);
            assert(!runtime.status && !converted.resultIsFloat && converted.result);
            assert(chali_abi_bytes(&frame, converted.result, &view, &length) && length);
            const std::string output(reinterpret_cast<const char *>(view), length);
            if (text == "{\"value\":null}") assert(output == "0TOML 不支持 null");
            else if (text == "[]" || text == "{") assert(output == "0TOML 序列化要求对象根节点");
            else assert(output[0] == '1');
            assert(chali_abi_string_release(&frame, converted.result));
            converted.result = 0;
            arena.remaining = 0;
            transcode(&converted);
            arena.remaining = -1;
            assert(runtime.status && !converted.result);
            reset();
            converted.count = 1;
            transcode(&converted);
            assert(runtime.status);
            reset();
            converted.count = 2;
            kinds[0] = ChaliAbiSlotFloat;
            transcode(&converted);
            assert(runtime.status);
            reset();
            kinds[0] = ChaliAbiSlotInteger;
            assert(chali_abi_string_release(&frame, arguments[0]));
            transcode(&converted);
            assert(runtime.status);
            reset();
        }
        std::puts("C format conversion: independent buffers, null rejection, long input and invalid frames passed");
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
        const auto allocationsBeforeBorrow = allocationEvents.load(std::memory_order_relaxed);
        for (int index = 0; index < 100; ++index)
            assert(*static_cast<int *>(chali_abi_record_borrow(&frame, record, "c-api-test::Integer")) == 42);
        const auto borrowAllocations = allocationEvents.load(std::memory_order_relaxed) - allocationsBeforeBorrow;
        const char *expectedEnvironment = std::getenv("CHALI_TEST_HOST_ALLOCATIONS");
        const auto expectedAllocations = expectedEnvironment ? std::strtoull(expectedEnvironment, nullptr, 10) : 0;
        std::printf("C host shells: %zu allocations for 100 warmed record borrows\n", borrowAllocations);
        std::fflush(stdout);
        assert(borrowAllocations == expectedAllocations);
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
        const auto automatic = chali_abi_record_make_clonable(&frame, "c-api-test::AutomaticInteger",
            new int(42), destroyInteger, cloneInteger);
        assert(automatic && !runtime.status);
        NativeCallContextImpl automaticHost(context, &invoker);
        int automaticFailures = 0;
        for (int point = 0; point < 32; ++point)
        {
            const auto before = destroyed;
            arena.remaining = point;
            std::int64_t attempted = 0;
            try { attempted = automaticHost.cloneNativeRecord(automatic); }
            catch (const std::bad_alloc &) {}
            arena.remaining = -1;
            if (attempted)
            {
                auto *value = static_cast<int *>(chali_abi_record_borrow(&frame, attempted,
                    "c-api-test::AutomaticInteger"));
                assert(value && *value == 42);
                *value = 91;
                assert(*static_cast<int *>(chali_abi_record_borrow(&frame, automatic,
                    "c-api-test::AutomaticInteger")) == 42);
                assert(chali_abi_record_release(&frame, attempted) && destroyed == before + 1);
                break;
            }
            ++automaticFailures;
            assert(destroyed == before + 1);
            reset();
            assert(point != 31);
        }
        assert(automaticFailures > 0);
        {
            TransferPacket packet;
            Memory::Set<std::int64_t> active;
            assert(captureTransferObject(context, automatic, packet, active));
            assert(packet.objects.size() == 1);
            *static_cast<int *>(packet.objects.at(automatic).nativeRecord.get()) = 63;
            Memory::Vector<std::int64_t> created;
            std::int64_t restored = 0;
            assert(restoreTransferObject(context, packet, automatic, active, created, &restored));
            assert(created.size() == 1 && restored != automatic);
            auto *value = static_cast<int *>(chali_abi_record_borrow(&frame, restored,
                "c-api-test::AutomaticInteger"));
            assert(value && *value == 63);
            *value = 78;
            assert(*static_cast<int *>(packet.objects.at(automatic).nativeRecord.get()) == 63);
            assert(*static_cast<int *>(chali_abi_record_borrow(&frame, automatic,
                "c-api-test::AutomaticInteger")) == 42);
            assert(chali_abi_record_release(&frame, restored));
        }
        int *conflictingPolicy = new int(7);
        assert(!chali_abi_record_make(&frame, "c-api-test::AutomaticInteger", conflictingPolicy, destroyInteger));
        delete conflictingPolicy;
        reset();
        assert(chali_abi_record_release(&frame, automatic));
        for (auto cloner : {failedClone, aliasedClone})
        {
            const char *name = cloner == failedClone ? "c-api-test::FailedClone" : "c-api-test::AliasedClone";
            const auto original = chali_abi_record_make_clonable(&frame, name, new int(17), destroyInteger, cloner);
            assert(original);
            const auto before = destroyed;
            assert(!automaticHost.cloneNativeRecord(original) && destroyed == before);
            reset();
            TransferPacket packet;
            Memory::Set<std::int64_t> active;
            assert(!captureTransferObject(context, original, packet, active) && destroyed == before);
            reset();
            assert(*static_cast<int *>(chali_abi_record_borrow(&frame, original, name)) == 17);
            assert(chali_abi_record_release(&frame, original) && destroyed == before + 1);
        }
        std::printf("automatic C record copy: %d allocation failure points cleaned\n", automaticFailures);
        const char binaryText[] = {'A', 0, char(255)};
        const auto rawString = chali_abi_string_bytes(&frame, binaryText, sizeof(binaryText));
        assert(rawString && !runtime.status);
        assert(chali_abi_bytes(&frame, rawString, &view, &length) && length == sizeof(binaryText));
        assert(std::memcmp(view, binaryText, length) == 0);
        assert(chali_abi_string_release(&frame, rawString));
        assert(!chali_abi_string_bytes(&frame, nullptr, 1) && runtime.status);
        reset();
        const unsigned char sourceBytes[] = {0, 255, 65};
        const auto byteList = chali_abi_list_from_bytes(&frame, sourceBytes, sizeof(sourceBytes));
        assert(byteList && !runtime.status);
        assert(chali_abi_list_bytes(&frame, byteList, nullptr, 0, &length) && length == 3);
        unsigned char outputBytes[] = {17, 17, 17};
        assert(!chali_abi_list_bytes(&frame, byteList, outputBytes, 2, &length) && length == 3 && runtime.status);
        assert(outputBytes[0] == 17 && outputBytes[1] == 17 && outputBytes[2] == 17);
        reset();
        assert(chali_abi_list_bytes(&frame, byteList, outputBytes, sizeof(outputBytes), &length));
        assert(length == 3 && std::memcmp(outputBytes, sourceBytes, length) == 0);
        assert(!chali_abi_list_bytes(&frame, byteList, outputBytes, sizeof(outputBytes), nullptr) && runtime.status);
        reset();
        assert(chali_abi_record_release(&frame, byteList));
        assert(!chali_abi_list_bytes(&frame, byteList, outputBytes, sizeof(outputBytes), &length) && runtime.status);
        reset();
        const auto emptyList = chali_abi_list_from_bytes(&frame, nullptr, 0);
        assert(emptyList && chali_abi_list_bytes(&frame, emptyList, nullptr, 0, &length) && length == 0);
        assert(chali_abi_record_release(&frame, emptyList));
        assert(!chali_abi_list_from_bytes(&frame, nullptr, 1) && runtime.status);
        reset();
        assert(!chali_abi_list_from_bytes(&frame, sourceBytes, SIZE_MAX) && runtime.status);
        reset();
        NativeCallContextImpl byteHost(context, &invoker);
        for (const auto &invalid : {NativeValue::fromInt(-1), NativeValue::fromInt(256), NativeValue::fromFloat(1.0), NativeValue::fromText(Memory::String("bad"))})
        {
            const auto invalidList = collection_glue::makeList(byteHost);
            assert(invalidList && collection_glue::listPush(byteHost, invalidList, invalid));
            outputBytes[0] = 17;
            assert(!chali_abi_list_bytes(&frame, invalidList, outputBytes, sizeof(outputBytes), &length) && runtime.status && outputBytes[0] == 17);
            reset();
            assert(chali_abi_record_release(&frame, invalidList));
        }
        const auto numericList = collection_glue::listStorageScalarOperation(byteHost, 9, 0, 0, 0, 0, 0, 0).bits;
        assert(numericList);
        collection_glue::listStorageScalarOperation(byteHost, 7, numericList, 255, 0, 0, 0, 0);
        assert(chali_abi_list_bytes(&frame, numericList, outputBytes, sizeof(outputBytes), &length) && length == 1 && outputBytes[0] == 255);
        assert(chali_abi_record_release(&frame, numericList));
        int byteCreationFailures = 0;
        for (int point = 0; point < 32; ++point)
        {
            arena.remaining = point;
            const auto attempted = chali_abi_list_from_bytes(&frame, sourceBytes, sizeof(sourceBytes));
            arena.remaining = -1;
            if (attempted) { assert(chali_abi_record_release(&frame, attempted)); break; }
            assert(runtime.status);
            ++byteCreationFailures;
            reset();
            assert(point != 31);
        }
        assert(byteCreationFailures >= 3);
        std::printf("C binary services: raw NUL bytes, atomic bounded output, numeric views and %d creation failure points passed\n", byteCreationFailures);
    }
    assert(arena.stats().liveBlocks == 0);
    std::puts("C services: buffered strings, memory ranges, borrowed invoker, errors and records passed");
}
