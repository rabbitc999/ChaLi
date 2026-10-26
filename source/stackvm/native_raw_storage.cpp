/**
 * @file native_raw_storage.cpp
 * @brief VM 与 AOT 共用的原始存储状态，不改变普通引用的存活验证。
 */
#include "runtime_internal.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 独占字节分配与逐元素初始化状态；语言层负责存活元素的确定性清理。
 */
struct RawStorageState
{
    HeapAllocator *heap = nullptr;
    ObjectStore *owner = nullptr;
    std::size_t address = 0;
    std::size_t allocation = 0;
    std::size_t stride = 0;
    std::int64_t tag = 0;
    Memory::Vector<unsigned char> initialized;
    /** @brief 分配失败或对象表收尾时回收尚未释放的字节块。 */
    ~RawStorageState()
    {
        if (owner && !owner->shuttingDown && address)
            owner->beginStorage(address, initialized.size() * stride, true);
        if (heap && allocation) heap->release(allocation);
    }
};

/**
 * @brief 为直接字节指令验证完整范围并取得短期物理视图，不执行任何写入。
 * @param ctx 活跃数据区与堆。
 * @param operation 7 为复制，8 为填充。
 * @param destination 目标逻辑地址。
 * @param sourceOrByte 源逻辑地址或填充字节。
 * @param bytes 字节数。
 * @param output 接收目标地址。
 * @param input 接收源地址。
 * @return 成功为真；失败在上下文中记录诊断。
 */
bool rawMemoryView(NativeContext &ctx, std::int64_t operation, std::int64_t destination,
                   std::int64_t sourceOrByte, std::int64_t bytes, unsigned char *&output, unsigned char *&input)
{
    output = input = nullptr;
    const auto fail = [&](const char *message) { ctx.error = message; return false; };
    if (operation != 7 && operation != 8) return fail("原始字节操作编号无效");
    if (bytes < 0) return fail("原始字节操作长度不能为负数");
    if (bytes == 0) return true;
    if (static_cast<std::uint64_t>(bytes) > std::numeric_limits<std::size_t>::max())
        return fail("原始字节操作长度超出宿主地址范围");
    if (destination <= 0 || (operation == 7 && sourceOrByte <= 0)) return fail("原始字节操作地址无效");
    if (operation == 8 && (sourceOrByte < 0 || sourceOrByte > 255)) return fail("原始字节填充值必须在 0 到 255 之间");
    const auto count = static_cast<std::size_t>(bytes);
    const auto borrow = [&](std::int64_t address) -> unsigned char * {
        const auto at = static_cast<std::size_t>(address);
        if (ctx.data && at < ctx.data->binary.size())
            return count <= ctx.data->binary.size() - at ? const_cast<unsigned char *>(ctx.data->binary.data()) + at : nullptr;
#if STACKVM_WITH_DEFAULT_HEAP
        auto *heap = dynamic_cast<Heap *>(ctx.heap);
        return heap ? heap->borrowBytes(at, count) : nullptr;
#else
        return nullptr;
#endif
    };
    output = borrow(destination);
    if (!output) return fail("原始字节操作目标越界或已释放");
    if (operation == 7)
    {
        input = borrow(sourceOrByte);
        if (!input) return fail("原始字节复制源越界或已释放");
        const auto distance = destination > sourceOrByte ? static_cast<std::uint64_t>(destination - sourceOrByte) : static_cast<std::uint64_t>(sourceOrByte - destination);
        if (distance < static_cast<std::uint64_t>(bytes)) return fail("memcpy 不允许重叠范围");
    }
    return true;
}

/**
 * @brief 在同一原生调用内验证操作并更新初始化状态，容量乘法先于分配检查。
 * @param ctx 活跃堆与对象表。
 * @param op 操作编号。
 * @param handle 分配句柄或字节目标地址。
 * @param index 元素下标、字节源地址或填充值。
 * @param value 操作值或字节数。
 * @param alignment 原始分配的对齐。
 * @param tag 受检槽位的类型身份；裸字节路径忽略。
 * @return 结果；失败通过上下文报告。
 */
Slot rawStorageOperation(NativeContext &ctx, std::int64_t op, std::int64_t handle, std::int64_t index,
                         std::int64_t value, std::int64_t alignment, std::int64_t tag)
{
    const auto fail = [&](const char *message) {
        ctx.error = message;
        return Slot::fromInt(0);
    };
    if (!ctx.heap || !ctx.objects) return fail("原始存储需要堆与对象表");
    if (op == 7 || op == 8)
    {
        unsigned char *destination = nullptr, *source = nullptr;
        if (!rawMemoryView(ctx, op, handle, index, value, destination, source) || value == 0) return Slot::fromInt(0);
        const auto count = static_cast<std::size_t>(value);
        if (op == 7)
            std::memcpy(destination, source, count);
        else
            std::memset(destination, static_cast<unsigned char>(index), count);
        return Slot::fromInt(0);
    }
    if (op == 0)
    {
#if !STACKVM_WITH_DEFAULT_HEAP
        return fail("原始存储需要支持对齐视图的默认字节堆");
#else
        if (index < 0 || value <= 0 || alignment <= 0 || alignment > 16 || (alignment & (alignment - 1)) != 0
            || value % alignment != 0 || tag <= 0)
            return fail("原始存储容量、步长、对齐或类型身份无效");
        if (static_cast<std::uint64_t>(index) > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) / static_cast<std::uint64_t>(value))
            return fail("原始存储容量乘法溢出");
        auto state = Memory::makeShared<RawStorageState>();
        state->heap = ctx.heap;
        state->owner = ctx.objects;
        state->stride = static_cast<std::size_t>(value);
        state->tag = tag;
        if (index)
        {
            const auto bytesRequested = static_cast<std::size_t>(index) * state->stride;
            const auto padding = static_cast<std::size_t>(alignment - 1);
            if (bytesRequested > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()) - padding)
                return fail("原始存储对齐容量溢出");
            state->allocation = nativeAllocate(ctx, bytesRequested + padding);
            if (!state->allocation) return fail("原始存储分配失败");
            auto *heap = dynamic_cast<Heap *>(ctx.heap);
            const auto *bytes = heap ? heap->borrowBytes(state->allocation, bytesRequested + padding) : nullptr;
            if (!bytes) return fail("原始存储分配器不支持对齐视图");
            const auto displacement = (static_cast<std::size_t>(alignment) - reinterpret_cast<std::uintptr_t>(bytes) % static_cast<std::size_t>(alignment)) % static_cast<std::size_t>(alignment);
            state->address = state->allocation + displacement;
            if (!heap->restrictAllocation(state->allocation, displacement, bytesRequested))
                return fail("原始存储对齐视图范围无效");
        }
        state->initialized.resize(static_cast<std::size_t>(index), 0);
        auto object = Memory::makeShared<RuntimeObject>();
        object->kind = ObjectKind::RawStorage;
        object->rawStorage = std::move(state);
        return Slot::fromInt(ctx.objects->add(std::move(object)));
#endif
    }
    auto *object = nativeObject(ctx, Slot::fromInt(handle), ObjectKind::RawStorage, "原始存储");
    if (!object || !object->rawStorage) return Slot::fromInt(0);
    auto &state = *object->rawStorage;
    // 裸地址仅验证分配与位置，不读取元素类型身份或初始化位图。
    if (op == 6)
    {
        if (index < 0 || static_cast<std::uint64_t>(index) > state.initialized.size())
            return fail("裸指针位置超出原始分配");
        return Slot::fromInt(static_cast<std::int64_t>(state.address + static_cast<std::size_t>(index) * state.stride));
    }
    if (tag != state.tag) return fail("原始指针元素类型与存储不匹配");
    if (op == 9)
        return Slot::fromInt(std::find(state.initialized.begin(), state.initialized.end(), 1) != state.initialized.end());
    if (op == 4) return Slot::fromInt(static_cast<std::int64_t>(state.initialized.size()));
    if (op == 5)
    {
        if (std::find(state.initialized.begin(), state.initialized.end(), 1) != state.initialized.end())
            return fail("释放原始存储前必须清理全部已初始化元素");
        ctx.objects->release(handle);
        return Slot::fromInt(0);
    }
    if (index < 0 || static_cast<std::uint64_t>(index) >= state.initialized.size())
        return fail("原始指针访问越界");
    auto &live = state.initialized[static_cast<std::size_t>(index)];
    if (op == 3) return Slot::fromInt(live);
    if (op == 1)
    {
        if (value != 0 && value != 1) return fail("原始存储初始化条件无效");
        if (live != value) return fail(value ? "原始存储元素尚未初始化" : "原始存储元素已经初始化");
        const auto address = state.address + static_cast<std::size_t>(index) * state.stride;
        if (!value) ctx.objects->beginStorage(address, state.stride);
        return Slot::fromInt(static_cast<std::int64_t>(address));
    }
    if (op == 2)
    {
        if ((value != 0 && value != 1) || live == value) return fail("原始存储初始化状态转换无效");
        if (!value)
        {
            const char zeros[256]{};
            const auto address = state.address + static_cast<std::size_t>(index) * state.stride;
            for (std::size_t offset = 0; offset < state.stride;)
            {
                const auto bytes = std::min<std::size_t>(sizeof(zeros), state.stride - offset);
                if (!ctx.heap->writeBytes(address + offset, zeros, bytes)) return fail("原始槽位清空失败");
                offset += bytes;
            }
        }
        live = static_cast<unsigned char>(value);
        return Slot::fromInt(0);
    }
    return fail("未知原始存储操作");
}

/**
 * @brief 将 VM 原生调用的六个槽转发到标量内核，不复制元素。
 * @param ctx 活跃执行上下文。
 * @param args 操作及五个操作参数。
 * @return 操作结果；错误保存在上下文中。
 */
Slot rawStorage(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return rawStorageOperation(ctx, nativeInt(args[0]), nativeInt(args[1]), nativeInt(args[2]),
                               nativeInt(args[3]), nativeInt(args[4]), nativeInt(args[5]));
}
}  // namespace stackvm::runtimeDetail
