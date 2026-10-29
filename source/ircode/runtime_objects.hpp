/**
 * @file runtime_objects.hpp
 * @brief VM 内部 runtime objects 模块的类型与接口。
 */
#pragma once
#include "runtime_context.hpp"
#include <array>

namespace stackvm::runtimeDetail
{
/**
 * @brief 对象句柄的起点（2^62）：编码堆地址与数据区地址均低于此边界。
 */
constexpr std::int64_t kObjectHandleBase = 4611686018427387904LL;

/**
 * @brief 对象种类。
 */
struct RawStorageState;

enum class ObjectKind
{
    NativeRecord,  ///< 不透明宿主记录，语义由胶水及描述符回调维护。
    Obj,           ///< 装箱格子（obj）：一个值 + 它的类型号
    RawStorage,    ///< 显式初始化状态与类型身份的原始存储。
    Closure,       ///< 类型化函数入口和可写借用的捕获环境
};

/**
 * @brief 种类名（诊断文案里用）。
 * @param kind 待处理对象的类别。
 * @retval 对应对象的指针。
 */
[[nodiscard]] const char *objectKindName(ObjectKind kind);

using ObjectValue = NativeStoredValue;

/**
 * @brief 一个运行期对象（几种里的一种，用同一个结构装下，省一套 union 的麻烦）。
 */
struct RuntimeObject
{
    const NativeRecordType *nativeType = nullptr;               ///< 宿主描述符，VM 不根据名称分派。
    std::shared_ptr<void> nativeRecord;                         ///< 不透明记录，仅由宿主回调访问及销毁。
    std::int64_t closureTarget = 0;                             ///< 数据区函数标签地址，不保存宿主函数地址。
    bool closureEnvironment = false;                            ///< 调用时是否附加环境句柄。
    bool closureCopy = false;                                   ///< 拥有捕获使用稳定单元，复制闭包时复制当前状态。
    Memory::Map<std::size_t, std::int64_t> captureDestructors;  ///< 聚合捕获的用户析构函数。
    ObjectKind kind = ObjectKind::NativeRecord;
    std::int64_t identity = 0;          ///< 值副本共享的键身份；新对象由 ObjectStore::add 分配。
    Memory::Vector<ObjectValue> items;  ///< 动态状态箱及闭包的拥有值槽。
    /**
     * @brief 活跃 foreach 元素引用的可寻址单元，由最外层借用负责回收。
     */
    struct ElementView
    {
        std::size_t address = 0;
        std::size_t users = 1;
        bool isText = false;
        bool isFloat = false;
        std::int64_t objectFlag = 0;
    };
    Memory::Map<std::size_t, ElementView> elementViews;
    bool refreshingViews = false;
    /**
     * Obj：**类型号**（由前端在编译期算好，`nameof(类型)` 给的就是它；0 = 还没 set 过）。
     * 有了类型号，`obj.name()` 才能和 `nameof(...)` 比对 —— 泛型就是靠这一条分支的。
     */
    std::int64_t objTag = 0;
    /**
     * @brief Obj：类型名（**只为诊断文案**：`obj.get` 取错类型时要说清里面装的是什么）。
     */
    Memory::String objTypeName;
    Memory::Vector<std::int64_t> objTextCallbacks;  ///< 文本覆盖方法及其字段存储布局。
    Memory::String objTextShape;                    ///< 默认值文本布局；复制 Any 时一并保存。
    /// Obj：装着的值（数值 / 字符串副本；装指针时就是那个地址）。
    ObjectValue objValue;
    /// Obj：objValue.integer 是不是一份由这个盒子持有的运行期对象句柄。
    bool objOwnsObject = false;
    std::int64_t iteratorLayout = 0;  ///< 状态叶子布局字符串地址。
    bool iteratorStateBox = false;    ///< 专用迭代状态箱拥有单元素存储。
    NativeContext iteratorContext;    ///< 迭代状态或 copy 捕获单元的堆与对象表，供复制和清理使用。
    std::shared_ptr<RawStorageState> rawStorage; ///< 仅 RawStorage 使用的存储及初始化状态。
};

bool nativeRefreshViews(NativeContext &ctx, RuntimeObject &object);
bool nativeWriteView(NativeContext &ctx, const RuntimeObject::ElementView &view, const ObjectValue &value);

void nativeReleaseViewContents(NativeContext &ctx, const RuntimeObject::ElementView &view);

/**
 * @brief 对象表：句柄 → 对象。
 */
struct ObjectStore
{
    bool pointerSnapshots = true; ///< 非拥有指针包装共享不可变快照，修改时分离。
    bool inlineHandles = true; ///< 少量活跃 Obj 使用固定槽，宿主记录及冲突沿原表回退。
    /** @brief 不搬动的内联句柄槽；完整句柄比较拒绝已释放或碰撞的旧身份。 */
    struct HandleSlot
    {
        std::int64_t handle = 0;
        std::shared_ptr<RuntimeObject> object;
    };
    std::array<HandleSlot, 32> handleSlots;
    std::size_t inlineHandleCount = 0; ///< 当前固定槽中的活跃对象数。
    /** @brief 读取指针快照和内联槽开关，不改变其他对象的复制协议。 */
    ObjectStore();
    NativeContext recordContext;                                                  ///< 不透明记录清理所需的运行时存储上下文。
    Memory::Map<std::pair<std::size_t, std::int64_t>, std::size_t> movedStorage;  ///< 已销毁子对象的（地址、类型号）及存储大小。
    std::size_t largestMovedStorage = 0; ///< 标记的最大跨度上界，用于限制重叠查询范围。
    Memory::Map<std::int64_t, std::shared_ptr<RuntimeObject>> objects;            ///< 还活着的对象
    /**
     * @brief 新分配或重新赋值的存储开始生命周期。
     * @param address 地址。
     * @param bytes 字节数。
     * @param fresh 是否为新分配块，需清除所有重叠的旧标记。
     */
    void beginStorage(std::size_t address, std::size_t bytes, bool fresh = false);
    bool shuttingDown = false;
    /**
     * @brief 释放对象存储中仍持有的全部资源。
     */
    ~ObjectStore();
    std::int64_t next = kObjectHandleBase;  ///< 下一个句柄
    Memory::Vector<std::size_t> materializedHeap;
    Memory::Vector<std::int64_t> materializedObjects;

    /**
     * @brief 收下一个对象，返回它的句柄。
     * @param object 待访问的对象。
     * @retval 收下一个对象，返回它的句柄。
     */
    std::int64_t add(std::shared_ptr<RuntimeObject> object);

    /**
     * @brief 根据名称或键查找匹配项。
     *
     * 按句柄找对象；已经释放 / 不认识返回 nullptr。
     * @param handle 待操作的运行时句柄。
     * @retval 匹配项的指针，未找到时为空指针。
     */
    RuntimeObject *find(std::int64_t handle);

    /**
     * @brief 查找完整句柄对应的稳定存储槽，不创建对象。
     * @param handle 对象的完整逻辑身份。
     * @return 对应共享指针槽；身份不存在时返回空。
     */
    std::shared_ptr<RuntimeObject> *handleStorage(std::int64_t handle);

    /**
     * @brief 在修改共享的非拥有指针包装前分离该句柄的快照。
     * @param handle 已验证的包装句柄。
     * @return 当前句柄独占的可修改包装；无效句柄返回空。
     */
    RuntimeObject *detachPointerSnapshot(std::int64_t handle);

    /**
     * @brief 复制对象值并维护其引用或所有权。
     * @param value 待转换、保存或参与计算的值。
     * @param error 接收失败原因的错误信息。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool copyValue(ObjectValue *value, Memory::String *error);

    /**
     * @brief 释放对象值持有的资源。
     * @param value 待转换、保存或参与计算的值。
     */
    void releaseValue(const ObjectValue &value);

    /**
     * @brief 释放通用对象拥有的槽值与宿主记录资源。
     * @param object 待访问的对象。
     */
    void releaseContents(const RuntimeObject &object);

    /**
     * @brief 复制运行时对象并调用宿主记录的复制协议。
     * @param object 待访问的对象。
     * @param error 接收失败原因的错误信息。
     * @retval 对应对象的指针。
     */
    std::shared_ptr<RuntimeObject> copyObject(const RuntimeObject &object, Memory::String *error);

    /**
     * @brief 复制运行时句柄并维护其对象引用。
     * @param handle 待操作的运行时句柄。
     * @param error 接收失败原因的错误信息。
     * @retval 复制运行时句柄并维护其对象引用。
     */
    std::int64_t copyHandle(std::int64_t handle, Memory::String *error);

    /**
     * @brief 释放指定地址对应的运行时资源。
     *
     * 释放一个对象；确实释放了返回 true。
     * @param handle 待操作的运行时句柄。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool release(std::int64_t handle);

    /**
     * @brief 还活着的对象个数（停机时 != 0 就是漏了）。
     * @retval 还活着的对象个数（停机时 != 0 就是漏了）。
     */
    [[nodiscard]] std::size_t liveCount() const;
};
/**
 * @brief 判断整数是否位于运行期对象句柄的保留区间。
 * @param handle 待判断的整数值。
 * @return 属于对象句柄区间时返回真。
 */
[[nodiscard]] bool isObjectHandle(std::int64_t handle);
}  // namespace stackvm::runtimeDetail
