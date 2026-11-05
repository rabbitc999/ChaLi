/**
 * @file chali_c_heap_allocator.cpp
 * @brief 核对宿主索引与容器的 C 堆路径不调用 C++ new。
 */
#include "../source/allocator/system_arena.hpp"
#include "cas/memory.h"
#include <cassert>
#include <vector>

static bool rejectCppAllocation = false;
/**
 * @brief 在被测区间阻止任何隐式 C++ 堆分配。
 * @param bytes 请求大小。
 * @return 测试初始化用存储；被测区间直接终止。
 */
void *operator new(std::size_t bytes)
{
    if (rejectCppAllocation) std::abort();
    if (void *result = std::malloc(bytes ? bytes : 1)) return result;
    throw std::bad_alloc();
}
/**
 * @brief 释放测试初始化时的 C++ 分配。
 * @param pointer 待释放存储。
 */
void operator delete(void *pointer) noexcept { std::free(pointer); }
/**
 * @brief 支持编译器生成的带大小释放。
 * @param pointer 待释放存储。
 * @param bytes 原请求大小。
 */
void operator delete(void *pointer, std::size_t bytes) noexcept { (void)bytes; std::free(pointer); }

/** @brief 需要超出 malloc 默认对齐的元素。 */
struct alignas(256) AlignedElement { unsigned char bytes[256]; };
/** @brief 验证基类子对象地址偏移时仍释放完整对象。 */
struct FirstBase { int first = 1; };
/** @brief 接收拥有型指针转换的基类，无虚析构。 */
struct SecondBase { int second = 2; };
/** @brief 用析构计数验证具体对象的释放。 */
struct Composite : FirstBase, SecondBase
{
    int *count;
    /** @brief 保存析构计数。 @param target 计数地址。 */
    explicit Composite(int *target) : count(target) {}
    /** @brief 记录一次完整对象析构。 */
    ~Composite() { ++*count; }
};
/** @brief 用于验证数组构造失败时逐元素回滚。 */
struct ThrowingElement
{
    inline static int remaining = 3, live = 0;
    /** @brief 第四个元素构造失败，之前的元素必须析构。 */
    ThrowingElement() { if (!remaining--) throw 7; ++live; }
    /** @brief 记录元素已经销毁。 */
    ~ThrowingElement() { --live; }
};

/**
 * @brief 核对 C 堆对齐、溢出拒绝及系统索引预算与身份。
 * @return 所有断言通过时返回零。
 */
int main()
{
    rejectCppAllocation = true;
    {
        std::vector<AlignedElement, Memory::CHeapAllocator<AlignedElement>> values(7);
        assert(reinterpret_cast<std::uintptr_t>(values.data()) % 256 == 0);
        values.resize(33);
        assert(reinterpret_cast<std::uintptr_t>(values.data()) % 256 == 0);
        int destroyed = 0;
        {
            Memory::CHeapUniquePtr<SecondBase> base = Memory::makeCHeapUnique<Composite>(&destroyed);
            assert(base->second == 2);
        }
        assert(destroyed == 1);
        auto bits = Memory::makeCHeapArray<bool>(17);
        assert(!bits[0] && !bits[16]);
        bits[3] = true;
        bool *bitPointer = bits.get();
        assert(bitPointer[3]);
        auto empty = Memory::makeCHeapArray<int>(0);
        assert(!empty);
        auto shared = std::allocate_shared<int>(Memory::CHeapAllocator<int>(), 42);
        assert(*shared == 42);
        bool constructorFailed = false;
        try { auto array = Memory::makeCHeapArray<ThrowingElement>(8); }
        catch (int value) { constructorFailed = value == 7; }
        assert(constructorFailed && ThrowingElement::live == 0);
        Cas::Memory::PoolAllocator<AlignedElement> casAllocator;
        auto *beforePool = casAllocator.allocate(3);
        assert(reinterpret_cast<std::uintptr_t>(beforePool) % 256 == 0);
        alignas(std::max_align_t) unsigned char casStorage[8192];
        assert(Cas::Allocator::initialize(casStorage, sizeof(casStorage)));
        casAllocator.deallocate(beforePool, 3);
        auto *afterPool = casAllocator.allocate(3);
        assert(reinterpret_cast<std::uintptr_t>(afterPool) % 256 == 0);
        casAllocator.deallocate(afterPool, 3);
        auto *overflowPool = casAllocator.allocate(100);
        assert(reinterpret_cast<std::uintptr_t>(overflowPool) % 256 == 0);
        casAllocator.deallocate(overflowPool, 100);
        assert(Cas::Allocator::quiescent());
        Memory::SystemRuntimeArena arena(1024);
        void *blocks[64];
        for (auto &block : blocks)
        {
            block = arena.allocateAligned(16, 64);
            assert(block && reinterpret_cast<std::uintptr_t>(block) % 64 == 0);
            assert(arena.owns(static_cast<char *>(block) + 15));
            assert(!arena.release(static_cast<char *>(block) + 1));
        }
        assert(!arena.allocate(1));
        assert(arena.stats().liveBytes == 1024 && arena.stats().liveBlocks == 64);
        for (auto block : blocks) assert(arena.release(block));
        assert(arena.stats().liveBytes == 0 && arena.stats().liveBlocks == 0);
        assert(arena.stats().peakBytes == 1024 && arena.stats().totalBlocks == 64);
        auto zero = arena.allocate(0);
        assert(zero && arena.owns(zero) && arena.release(zero));
    }
    rejectCppAllocation = false;
    Memory::CHeapAllocator<AlignedElement> allocator;
    bool rejected = false;
    try { allocator.allocate(std::numeric_limits<std::size_t>::max()); }
    catch (const std::bad_alloc &) { rejected = true; }
    assert(rejected);
}
