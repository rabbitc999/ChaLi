/**
 * @file chali_object_construction_scalar.cpp
 * @brief 核对标量对象建立、设置及刷新后复制的语义与错误边界。
 */
#include "ircode/runtime_internal.hpp"
#include "allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>

/**
 * @brief 核对元数据、快照分离、旧载荷释放、失效诊断与元素视图刷新。
 * @return 全部断言通过时返回零。
 */
int main()
{
    using namespace stackvm;
    using namespace stackvm::runtimeDetail;
    Memory::SystemRuntimeArena arena(4 * 1024 * 1024);
    Memory::AllocationScope allocation(&arena);
    {
        DataArea data;
        data.binary.resize(128);
        const char name[] = "S const&";
        const char shape[] = "R0:0:0:8:0:0:";
        std::memcpy(data.binary.data() + 8, name, sizeof(name));
        std::memcpy(data.binary.data() + 40, shape, sizeof(shape));
        ObjectStore store;
        NativeContext context;
        context.data = &data;
        context.objects = &store;
        const auto first = nativeNewObjectHandle(context, ObjectKind::Obj);
        const auto second = nativeNewObject(context, ObjectKind::Obj).integer;
        assert(first != second && store.find(first)->objTag == 0);
        assert(store.find(first)->objTypeName.empty() && store.find(first)->objTextShape.empty());
        assert(setObjectInteger(context, first, 72, 17, 8, 40));
        Memory::Vector<Slot> args{ Slot::fromInt(second), Slot::fromInt(72), Slot::fromInt(17),
                                   Slot::fromInt(0), Slot::fromInt(0), Slot::fromInt(8), Slot::fromInt(40) };
        objSet(context, args);
        assert(context.error.empty() && context.memoryFault.empty());
        assert(store.find(first)->objTag == store.find(second)->objTag);
        assert(store.find(first)->objValue.integer == store.find(second)->objValue.integer);
        assert(store.find(first)->objTypeName == store.find(second)->objTypeName);
        assert(store.find(first)->objTextShape == store.find(second)->objTextShape);
        assert(!store.find(first)->objOwnsObject && !store.find(first)->objValue.ownsObject);

        // 复制仍建立独立句柄；标量设置不能修改另一个句柄持有的共享快照。
        const auto copy = copyObjectHandle(context, first);
        assert(copy && copy != first && store.find(copy)->objValue.integer == 72);
        assert(setObjectInteger(context, first, 96, 19, 8, 40));
        assert(store.find(copy)->objValue.integer == 72 && store.find(copy)->objTag == 17);
        assert(store.find(first)->objValue.integer == 96 && store.find(first)->objTag == 19);

        // 元数据失败不能先分离或改写载荷；标量及通用入口保持相同诊断。
        for (const bool badName : { true, false })
        {
            context.error.clear();
            context.memoryFault.clear();
            assert(!setObjectInteger(context, first, 111, 21, badName ? 500 : 8, badName ? 40 : 500));
            const auto scalarError = context.error;
            const auto scalarFault = context.memoryFault;
            context.error.clear();
            context.memoryFault.clear();
            args[0] = Slot::fromInt(first);
            args[1] = Slot::fromInt(111);
            args[2] = Slot::fromInt(21);
            args[5] = Slot::fromInt(badName ? 500 : 8);
            args[6] = Slot::fromInt(badName ? 40 : 500);
            objSet(context, args);
            assert(!scalarError.empty() && context.error == scalarError && context.memoryFault == scalarFault);
            assert(store.find(first)->objValue.integer == 96 && store.find(first)->objTag == 19);
        }
        context.error.clear();
        context.memoryFault.clear();
        args[5] = Slot::fromInt(8);
        args[6] = Slot::fromInt(40);

        // 重新绑定前仍需释放原有拥有载荷；空布局也沿用六参数入口的效果。
        const auto child = nativeNewObjectHandle(context, ObjectKind::Obj);
        const auto owner = nativeNewObjectHandle(context, ObjectKind::Obj);
        store.find(owner)->objValue.integer = child;
        store.find(owner)->objValue.ownsObject = true;
        store.find(owner)->objOwnsObject = true;
        assert(setObjectInteger(context, owner, 64, 23, 8, 0));
        assert(store.find(child) == nullptr && store.find(owner)->objTextShape.empty());

        // 调用者上下文的元素视图必须刷新，空视图以外的路径不能跳过检查。
        const auto viewed = nativeNewObjectHandle(context, ObjectKind::Obj);
        auto *object = store.find(viewed);
        object->items.resize(1);
        object->items[0].integer = 3;
        object->elementViews[0].address = 80;
        const std::int64_t current = 42;
        std::memcpy(data.binary.data() + 80, &current, sizeof(current));
        const auto refreshed = copyObjectHandle(context, viewed);
        assert(refreshed && refreshed != viewed);
        assert(store.find(viewed)->items[0].integer == 42 && store.find(refreshed)->items[0].integer == 42);
        object->elementViews[1].address = 80;
        context.error.clear();
        assert(copyObjectHandle(context, viewed) == 0 && context.error == "迭代期间列表结构已改变");
        object->elementViews.clear();
        context.error.clear();
        assert(store.release(viewed) && store.release(refreshed));
        assert(store.release(first) && store.release(second) && store.release(copy) && store.release(owner));
        assert(store.liveCount() == 0);
        assert(!setObjectInteger(context, first, 1, 1, 8, 40) && !context.error.empty());
        context.error.clear();
        assert(copyObjectHandle(context, first) == 0 && !context.error.empty());
    }
    assert(arena.stats().liveBlocks == 0);
    std::puts("object construction scalar: metadata, COW, errors, refreshed views and independent handles OK");
}
