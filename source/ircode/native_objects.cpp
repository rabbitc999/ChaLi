/**
 * @file native_objects.cpp
 * @brief 通用对象、Any 装箱与接口调用。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief `__std_obj_new() -> obj`：造一个空格子（类型号 0 = 还没 set 过）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot objNew(NativeContext &ctx, const Memory::Vector<Slot> &)
{
    return nativeNewObject(ctx, ObjectKind::Obj);
}

/**
 * @brief 接管独立聚合存储，复制与销毁通过语言函数调用约定执行。
 * @param ctx 当前运行上下文。
 * @param args 聚合地址、具体类型号、复制助手和销毁助手。
 * @return 新接口句柄。
 */
Slot interfaceBox(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    auto object = Memory::makeShared<RuntimeObject>();
    object->kind = ObjectKind::Obj;
    object->objValue.integer = nativeInt(args[0]);
    object->objTag = nativeInt(args[1]);
    object->interfaceCopy = nativeInt(args[2]);
    object->interfaceDrop = nativeInt(args[3]);
    return Slot::fromInt(ctx.objects->add(std::move(object)));
}

/**
 * @brief 借用接口值的稳定聚合地址，不复制或销毁来源载荷。
 * @param ctx 当前运行上下文。
 * @param args 原接口对象句柄。
 * @return 独立引用包装句柄，来源无效时返回零并记录错误。
 */
Slot interfaceBorrow(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto *source = nativeObject(ctx, args[0], ObjectKind::Obj, "接口值借用");
    if (!source) return Slot::fromInt(0);
    auto reference = Memory::makeShared<RuntimeObject>();
    reference->kind = ObjectKind::Obj;
    reference->objTag = source->objTag;
    reference->objValue.integer = source->objValue.integer;
    reference->objTextShape = "R";
    return Slot::fromInt(ctx.objects->add(std::move(reference)));
}

/**
 * @brief 读取元数据、分离共享快照并替换已验证对象的载荷。
 * @param ctx 当前原生上下文。
 * @param handle 已通过类别、存活和视图刷新检查的目标句柄。
 * @param value 已完成通用值转换的载荷。
 * @param tag 新的具体类型编号。
 * @param ownsObject 是否接管载荷中的对象句柄。
 * @param typeName 类型名的逻辑字符串地址。
 * @param shape 文本布局的逻辑字符串地址。
 * @param callbacks 可选的原实参列表，第七项之后按原协议读取文本回调。
 * @return 完成设置时为真；失败时保留原有错误及更新顺序。
 */
static bool assignObjectValue(NativeContext &ctx, std::int64_t handle, ObjectValue &&value,
                              std::int64_t tag, bool ownsObject, std::int64_t typeName,
                              std::int64_t shape, const Memory::Vector<Slot> *callbacks)
{
    Memory::String name;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(typeName), &name))
    {
        ctx.error = "obj.set 读不到类型名（地址 " + Memory::toString(typeName) + "）";
        return false;
    }
    Memory::String textShape;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(shape), &textShape))
    {
        ctx.error = "obj.set 读不到文本布局";
        return false;
    }
    auto *object = ctx.objects->detachPointerSnapshot(handle);
    if (object == nullptr)
    {
        nativeObject(ctx, handle, ObjectKind::Obj, "obj.set");
        return false;
    }
    ctx.objects->invalidateNativeRecordViews();
    object->objTextShape = std::move(textShape);
    object->objTextCallbacks.clear();
    if (callbacks)
        for (std::size_t index = 7; index < callbacks->size(); ++index) object->objTextCallbacks.push_back(nativeInt((*callbacks)[index]));
    if (object->objOwnsObject) ctx.objects->release(object->objValue.integer);
    object->objTag = tag;
    object->objTypeName = std::move(name);
    object->objValue = std::move(value);
    object->objOwnsObject = ownsObject;
    object->objValue.ownsObject = object->objOwnsObject;
    return true;
}

/**
 * @brief 把通用值装入对象，保持旧值释放、元数据与回调的原有协议。
 * @param ctx 当前原生上下文。
 * @param args 对象、载荷、类型号、字符串及对象标志、类型名和可选布局与回调。
 * @return 原有整数零结果；失败通过上下文报告。
 */
Slot objSet(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    if (args.size() != 6u && (args.size() < 7u || (args.size() - 7u) % 2u))
    {
        ctx.error = "obj.set 的内部文本回调参数不完整";
        return Slot::fromInt(0);
    }
    if (nativeObject(ctx, args[0], ObjectKind::Obj, "obj.set") == nullptr) return Slot::fromInt(0);
    ObjectValue value;
    if (!nativeValue(ctx, args[1], nativeInt(args[3]) != 0, nativeInt(args[4]), false,
                     &value)) return Slot::fromInt(0);
    const bool ownsObject = nativeInt(args[4]) == 1 || (nativeInt(args[4]) == -1 && isObjectHandle(value.integer));
    assignObjectValue(ctx, nativeInt(args[0]), std::move(value), nativeInt(args[2]), ownsObject,
                      nativeInt(args[5]), args.size() >= 7u ? nativeInt(args[6]) : 0, &args);
    return Slot::fromInt(0);
}

/**
 * @brief 直接设置整数载荷，省去通用参数向量及值槽转换。
 * @param ctx 当前原生上下文。
 * @param handle 目标对象的完整句柄。
 * @param value 非拥有整数载荷。
 * @param tag 具体类型编号。
 * @param typeName 类型名的逻辑字符串地址。
 * @param textShape 文本布局的逻辑字符串地址。
 * @return 设置成功时为真；失败保留与通用入口相同的诊断。
 */
bool setObjectInteger(NativeContext &ctx, std::int64_t handle, std::int64_t value,
                      std::int64_t tag, std::int64_t typeName, std::int64_t textShape)
{
    if (nativeObject(ctx, handle, ObjectKind::Obj, "obj.set") == nullptr) return false;
    ObjectValue payload;
    payload.integer = value;
    return assignObjectValue(ctx, handle, std::move(payload), tag, false, typeName, textShape, nullptr);
}

/**
 * @brief `obj.get` / `obj.get` 取指针 共用的**类型校验**。
 *
 * 前端把"这个位置上允许出现的类型号"全部算好跟在后面（`args[4..]`）：
 * 目标是具体类型时就是它自己，目标是接口指针时是"所有实现了这个接口的结构体指针"。
 * 对不上就写 `ctx.error`（`run()` 会转成运行期错误）。
 *
 * @param ctx 环境。
 * @param object 盒子。
 * @param args 实参：`[1]` 要取的类型的名字地址、`[2]`/`[3]`/`[4]` 形状标志、`[5..]` 允许的类型号。
 * @param who 谁在取（诊断文案）。
 * @return 允许返回 true。
 */
bool objCheckTag(NativeContext &ctx, const RuntimeObject *object, const Memory::Vector<Slot> &args,
                 const char *who)
{
    for (std::size_t index = 5u; index < args.size(); ++index)
    {
        if (nativeInt(args[index]) == object->objTag) return true;
    }
    Memory::String wanted = "?";
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[1])), &wanted)) wanted = "?";
    const Memory::String inside = object->objTypeName.empty() ? Memory::String("空盒子") : object->objTypeName;
    ctx.error = Memory::String(who) + "：类型对不上 —— Any 中是 " + inside + "，期望 " + wanted + "。请使用 isType<T> 或 tryGet<T>";
    return false;
}

/**
 * @brief `__std_obj_get(obj, 要取的类型的名字地址, 是不是字符串, 是不是浮点, 是不是对象句柄, 允许的类型号…) -> 值`。
 *
 * 取出来的是**裸值**（整数 / 浮点 / 堆上的新串 / 指针的地址）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot objGet(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    RuntimeObject *object = nativeObject(ctx, args[0], ObjectKind::Obj, "obj.get");
    if (object == nullptr) return Slot::fromInt(0);
    if (!objCheckTag(ctx, object, args, "obj.get")) return Slot::fromInt(0);
    const ObjectValue &value = object->objValue;
    if (nativeInt(args[2]) != 0)
    {
        if (!value.isText)
        {
            ctx.error = "obj.get：obj 里装的不是字符串，取不出字符串";
            return Slot::fromInt(0);
        }
        return Slot::fromInt(static_cast<std::int64_t>(nativeAllocString(ctx, value.text)));
    }
    if (nativeInt(args[3]) != 0) return Slot::fromDouble(value.real);
    if (nativeInt(args[4]) != 0 && object->objOwnsObject)
    {
        Memory::String error;
        const std::int64_t copy = ctx.objects->copyHandle(value.integer, &error);
        if (copy == 0)
        {
            ctx.error = error;
            return Slot::fromInt(0);
        }
        return Slot::fromInt(copy);
    }
    return Slot::fromInt(value.integer);
}

/**
 * @brief 判断对象是否带有指定类型标记。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot objHasTag(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    RuntimeObject *object = ctx.objects->find(nativeInt(args[0]));
    if (object == nullptr || object->kind != ObjectKind::Obj) return Slot::fromInt(0);
    for (std::size_t index = 1u; index < args.size(); ++index)
        if (nativeInt(args[index]) == object->objTag) return Slot::fromInt(1);
    return Slot::fromInt(0);
}

/**
 * @brief `__std_obj_get_ptr(obj, 要取的类型的名字地址, 0, 0, 1, 允许的类型号…) -> 新的 obj（胖指针）`。
 *
 * 接口指针走这一条：校验过关后**复制一份胖指针**（值语义），交给变量持有 ——
 * 于是"两个接口指针"各拿各的类型号，谁先出块被释放都不会把对方弄悬空。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot objGetPointer(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    RuntimeObject *object = nativeObject(ctx, args[0], ObjectKind::Obj, "obj.get");
    if (object == nullptr) return Slot::fromInt(0);
    if (!objCheckTag(ctx, object, args, "obj.get")) return Slot::fromInt(0);
    Memory::String error;
    if (object->iteratorStateBox)
    {
        auto found = object->elementViews.find(0);
        if (found == object->elementViews.end())
        {
            RuntimeObject::ElementView view;
            view.objectFlag = object->iteratorLayout;
            view.address = nativeAllocate(ctx, std::max<std::size_t>(8, object->items[0].fields.size() * 8));
            if (!view.address)
            {
                ctx.error = "迭代器状态存储不足";
                return Slot::fromInt(0);
            }
            if (!nativeWriteView(ctx, view, object->items[0]))
            {
                nativeReleaseViewContents(ctx, view);
                ctx.heap->release(view.address);
                return Slot::fromInt(0);
            }
            found = object->elementViews.emplace(0, view).first;
        }
        auto pointer = Memory::makeShared<RuntimeObject>();
        pointer->kind = ObjectKind::Obj;
        pointer->objTag = object->objTag;
        pointer->objTypeName = object->objTypeName;
        pointer->objValue.integer = static_cast<std::int64_t>(found->second.address);
        return Slot::fromInt(ctx.objects->add(std::move(pointer)));
    }
    std::shared_ptr<RuntimeObject> copy = ctx.objects->copyObject(*object, &error);
    if (!copy)
    {
        ctx.error = error;
        return Slot::fromInt(0);
    }
    return Slot::fromInt(ctx.objects->add(std::move(copy)));
}

/**
 * @brief `__std_obj_name(obj) -> 类型号`：和 `nameof(类型)` 是同一个编号（空格子给 0）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot objName(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    RuntimeObject *object = nativeObject(ctx, args[0], ObjectKind::Obj, "obj.name");
    if (object == nullptr) return Slot::fromInt(0);
    return Slot::fromInt(object->objTag);
}

/**
 * @brief 验证 CHALI 引用非空，阻止未绑定存储暴露为有效引用。
 * @param ctx 运行上下文。
 * @param args 单个引用地址或接口句柄。
 * @return 原引用；零地址时设置运行错误。
 */
Slot requireReference(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto address = nativeInt(args[0]);
    if (address == 0) ctx.error = "引用尚未绑定有效元素；缺省值请使用 Option";
    return Slot::fromInt(address);
}

/**
 * @brief `__std_obj_addr(obj) -> 地址`：胖指针里那个对象的地址（接口分派把它当 `self` 传下去）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot objAddress(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    RuntimeObject *object = nativeObject(ctx, args[0], ObjectKind::Obj, "接口指针");
    if (object == nullptr) return Slot::fromInt(0);
    return Slot::fromInt(object->objValue.integer);
}

/**
 * @brief `__std_iface_fail(类型号, 接口.方法 名地址) -> 0`：`q.area()` 分派不到实现时的报错出口。
 *
 * 两种情形都落到这里：接口指针是空的（类型号 0），或者它指着的对象没实现这个接口。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot ifaceFail(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    Memory::String who = "接口方法";
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[1])), &who))
    {
        who = "接口方法";
    }
    const std::int64_t tag = nativeInt(args[0]);
    ctx.error = "接口分派失败：" + who + (tag == 0 ? " 的接口指针是空的（没指向任何对象）" : " 没有类型号 " + Memory::toString(tag) + " 的实现");
    return Slot::fromInt(0);
}
}  // namespace runtimeDetail
}  // namespace stackvm
