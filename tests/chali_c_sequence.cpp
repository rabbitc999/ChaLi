/**
 * @file chali_c_sequence.cpp
 * @brief 对象 C 序列的所有者、失败回滚和生命周期检查。
 */
#include "../source/allocator/c_sequence.hpp"
#include "../source/allocator/c_index.hpp"
#include "../source/allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>

/** @brief 在全部被测操作期间禁止默认 C++ 堆分配。 */
static bool rejectCppAllocation;
/**
 * @brief 拦截默认 C++ 分配，运行时存储必须走 C 分配接口。
 * @param bytes 请求字节数。
 * @return 初始化存储。
 */
void *operator new(std::size_t bytes)
{
    if (rejectCppAllocation) std::abort();
    if (void *memory = std::malloc(bytes ? bytes : 1)) return memory;
    throw std::bad_alloc();
}
/**
 * @brief 释放初始化存储。
 * @param memory 地址。
 */
void operator delete(void *memory) noexcept { std::free(memory); }
/**
 * @brief 释放带大小的初始化存储。
 * @param memory 地址。
 * @param bytes 大小。
 */
void operator delete(void *memory, std::size_t bytes) noexcept
{
    (void)bytes;
    std::free(memory);
}

/** @brief 拥有独立 C 负载和自身地址，检测裸字节搬运与重复销毁。 */
struct Element
{
    inline static int live = 0, copiesBeforeFailure = -1;
    Element *self = this;
    int *payload;
    /**
     * @brief 构造独立负载。
     * @param value 值。
     */
    explicit Element(int value = 0) : payload(static_cast<int *>(std::malloc(sizeof(int))))
    {
        assert(payload);
        *payload = value;
        ++live;
    }
    /**
     * @brief 复制负载，在指定次数后失败。
     * @param source 原对象。
     */
    Element(const Element &source) : payload(nullptr)
    {
        if (copiesBeforeFailure == 0) throw 17;
        if (copiesBeforeFailure > 0) --copiesBeforeFailure;
        assert(source.self == &source && source.payload);
        payload = static_cast<int *>(std::malloc(sizeof(int)));
        assert(payload);
        *payload = *source.payload;
        ++live;
    }
    /**
     * @brief 移动负载，记录新对象地址。
     * @param source 原对象。
     */
    Element(Element &&source) noexcept : payload(source.payload)
    {
        assert(source.self == &source);
        source.payload = nullptr;
        ++live;
    }
    /** @brief 释放独立负载，验证移动后仍逐元素销毁。 */
    ~Element()
    {
        assert(self == this);
        std::free(payload);
        --live;
    }
};

/**
 * @brief 验证容器移动、嵌套、复制失败、预算失败、别名追加及所有者切换。
 * @return 断言通过返回零。
 */
int main()
{
    Memory::SystemRuntimeArena first(1024 * 1024), second(1024 * 1024), limited(96);
    rejectCppAllocation = true;
    {
        Memory::AllocationScope scope(&first);
        Memory::CSequence<Element> values;
        for (int i = 0; i < 257; ++i) values.emplace_back(i);
        assert(Element::live == 257 && values.capacity() == 512);
        for (int i = 0; i < 257; ++i) assert(values[i].self == &values[i] && *values[i].payload == i);
        values.reserve(1024);
        assert(Element::live == 257);
        {
            Memory::AllocationScope other(&second);
            Memory::CSequence<Element> copy(values);
            assert(copy.get_allocator().owner == &second && copy.size() == 257);
            assert(*copy[100].payload == 100 && copy[100].payload != values[100].payload);
            copy.reserve(600);
            const auto before = second.stats().liveBlocks;
            for (int fail = 0; fail < 257; ++fail)
            {
                Element::copiesBeforeFailure = fail;
                try
                {
                    copy = values;
                    assert(false);
                } catch (int error) { assert(error == 17); }
                assert(copy.size() == 257 && copy.capacity() == 600 && *copy[100].payload == 100);
                assert(second.stats().liveBlocks == before && Element::live == 514);
            }
            Element::copiesBeforeFailure = -1;
            copy = values;
            assert(copy.capacity() == 600 && copy.get_allocator().owner == &second);
            Memory::CSequence<Element> moved{ Memory::Allocator<Element>(&first) };
            moved = std::move(copy);
            assert(copy.empty() && copy.get_allocator().owner == &second && moved.get_allocator().owner == &second);
            copy.emplace_back(999);
            assert(copy.size() == 1 && *copy[0].payload == 999);
        }
        assert(second.stats().liveBlocks == 0 && Element::live == 257);
        Memory::CSequence<Memory::CSequence<Element>> nested;
        nested.emplace_back(std::move(values));
        assert(values.empty() && nested[0].size() == 257 && Element::live == 257);
        nested.clear();
        assert(Element::live == 0);
        Memory::CSequence<Element> aliases;
        aliases.emplace_back(42);
        aliases.push_back(aliases[0]);
        aliases.push_back(std::move(aliases[0]));
        assert(!aliases[0].payload && *aliases[1].payload == 42 && *aliases[2].payload == 42);
        aliases.clear();
        Memory::CIndex index;
        for (std::size_t key = 0; key < 1024; ++key) index.set(key, key * 17);
        {
            Memory::AllocationScope other(&second);
            Memory::CIndex copy(index);
            for (std::size_t key = 0; key < 1024; ++key)
            {
                std::size_t value = 0;
                assert(copy.get(key, value) && value == key * 17);
                if (key % 3 == 0) copy.remove(key);
            }
            Memory::CIndex moved(std::move(copy));
            std::size_t value = 0;
            assert(!moved.get(0, value) && moved.get(1, value) && value == 17);
        }
        assert(second.stats().liveBlocks == 0);
        {
            Memory::SystemRuntimeArena tight(192);
            Memory::CSequence<std::int64_t> slots{ Memory::Allocator<std::int64_t>(&tight) };
            slots.assign(16, 1);
            auto *address = slots.data();
            slots.assign(16, 0);
            assert(slots.data() == address && tight.stats().liveBlocks == 1 && slots[15] == 0);
            slots.assign(4, 9);
            slots.assign(12, 7);
            assert(slots.data() == address && slots.size() == 12 && slots[11] == 7);
            Memory::CSequence<std::int64_t> source;
            source.assign(16, 42);
            slots = source;
            assert(slots.data() == address && slots.size() == 16 && slots[15] == 42);
        }
        Memory::CSequence<Element> bounded{ Memory::Allocator<Element>(&limited) };
        bounded.emplace_back(7);
        Element external(81);
        auto *original = external.payload;
        bounded.emplace_back(8);
        bounded.emplace_back(9);
        bounded.emplace_back(10);
        try
        {
            bounded.push_back(std::move(external));
            assert(false);
        } catch (const std::bad_alloc &) {}
        assert(bounded.size() == 4 && external.payload == original && *external.payload == 81);
        try
        {
            bounded.reserve(1000);
            assert(false);
        } catch (const std::bad_alloc &) {}
        assert(bounded.size() == 4 && *bounded[0].payload == 7 && external.payload == original);
        assert(limited.stats().liveBlocks == 1);
    }
    assert(Element::live == 0 && first.stats().liveBlocks == 0 && second.stats().liveBlocks == 0 && limited.stats().liveBlocks == 0);
    puts("C sequence: nested ownership, 257 copy failures, budget rollback, aliases and C allocation passed");
}
