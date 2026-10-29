#pragma once
#include "stackvm.hpp"

namespace collection_glue
{
using NativeValue = stackvm::NativeValue;

/**
 * @brief 集合原语的数值结果，直接保留位模式与种类，不构造拥有文本的通用值。
 */
struct ScalarResult
{
    std::int64_t bits = 0;       ///< 整数值或浮点数的原始位模式。
    unsigned char floating = 0;  ///< 浮点数为一，整数为零。
};

/** @brief LLVM 数值数组视图，代次由所属运行时对象表拥有；上下文退出前清空缓存。 */
struct NumericListView
{
    std::int64_t handle = 0;
    void *header = nullptr;
    const std::uint64_t *revision = nullptr;
    std::uint64_t stamp = 0;
};

/**
 * @brief 查询已验证调用之后可直接访问的数值数组，不物化或改变集合。
 * @param host 当前原生上下文。
 * @param handle 目标集合句柄。
 * @param view 接收数值头部和失效代次，不可用时清空。
 */
void numericListView(stackvm::NativeCallContext &host, std::int64_t handle, NumericListView *view);

/**
 * @brief 直接执行自举 List 的单槽存储原语，避免原生函数查找及动态实参向量。
 * @param host 当前运行时的内存、对象与同步回调上下文。
 * @param operation 存储原语编号，零至十八；十三为租约，十四至十六比较及复制，十七查询上限，十八合并数值追加。
 * @param first 句柄或第一个标量实参。
 * @param second 第二个实参的原始位模式。
 * @param third 第三个实参的原始位模式。
 * @param fourth 第四个实参的原始位模式。
 * @param fifth 第五个实参的原始位模式。
 * @param floating 各实参是否为浮点位模式的位图。
 * @return 原语的整数或浮点结果；失败写入当前运行时诊断。
 */
NativeValue listStorageOperation(stackvm::NativeCallContext &host, std::int64_t operation,
                                 std::int64_t first, std::int64_t second, std::int64_t third,
                                 std::int64_t fourth, std::int64_t fifth, std::uint64_t floating);

/**
 * @brief 以无所有权数值结果返回：直接执行自举 List 的单槽存储原语，避免原生函数查找及动态实参向量。
 * @param host 当前运行时的内存、对象与同步回调上下文。
 * @param operation 存储原语编号，零至十六；十三为租约，十四至十六为单槽比较及复制。
 * @param first 句柄或第一个标量实参。
 * @param second 第二个实参的原始位模式。
 * @param third 第三个实参的原始位模式。
 * @param fourth 第四个实参的原始位模式。
 * @param fifth 第五个实参的原始位模式。
 * @param floating 各实参是否为浮点位模式的位图。
 * @return 原语的整数或浮点结果；失败写入当前运行时诊断。
 */
ScalarResult listStorageScalarOperation(stackvm::NativeCallContext &host, std::int64_t operation,
                                 std::int64_t first, std::int64_t second, std::int64_t third,
                                 std::int64_t fourth, std::int64_t fifth, std::uint64_t floating);

/**
 * @brief 以标量 ABI 执行自举 Map/Set 的槽位及所有权封送原语。
 * @param host 当前运行时的内存、对象与同步回调上下文。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param sixth 第六个实参位模式。
 * @param seventh 第七个实参位模式。
 * @param floating 浮点实参位图。
 * @return 原语结果；失败写入当前运行时诊断。
 */
NativeValue mapStorageOperation(stackvm::NativeCallContext &host, std::int64_t operation,
                                std::int64_t first, std::int64_t second, std::int64_t third,
                                std::int64_t fourth, std::int64_t fifth, std::int64_t sixth,
                                std::int64_t seventh, std::uint64_t floating);

/**
 * @brief 以无所有权数值结果返回：以标量 ABI 执行自举 Map/Set 的槽位及所有权封送原语。
 * @param host 当前运行时的内存、对象与同步回调上下文。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param sixth 第六个实参位模式。
 * @param seventh 第七个实参位模式。
 * @param floating 浮点实参位图。
 * @return 原语结果；失败写入当前运行时诊断。
 */
ScalarResult mapStorageScalarOperation(stackvm::NativeCallContext &host, std::int64_t operation,
                                std::int64_t first, std::int64_t second, std::int64_t third,
                                std::int64_t fourth, std::int64_t fifth, std::int64_t sixth,
                                std::int64_t seventh, std::uint64_t floating);

/**
 * @brief 创建原生调用可用的运行时列表。
 * @retval 创建原生调用可用的运行时列表。
 * @param host 当前调用所属的公开运行时上下文。
 */
std::int64_t makeList(stackvm::NativeCallContext &host);

/**
 * @brief 将一个原生值追加到运行时列表。
 * @param listHandle 目标运行时列表的句柄。
 * @param value 待转换、保存或参与计算的值。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool listPush(stackvm::NativeCallContext &host, std::int64_t listHandle, const NativeValue &value);

/**
 * @brief 将结构体数据追加到运行时列表。
 * @param listHandle 目标运行时列表的句柄。
 * @param structAddress 结构体数据的运行时地址。
 * @param slotCount 结构体数据包含的值槽数量。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool listPushStruct(stackvm::NativeCallContext &host, std::int64_t listHandle, std::int64_t structAddress,
                    std::size_t slotCount);

/**
 * @brief 从列表项读取结构体数据。
 * @param handle 待操作的运行时句柄。
 * @param slotCount 结构体数据包含的值槽数量。
 * @param out 接收处理结果的输出对象。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool readListStruct(stackvm::NativeCallContext &host, std::int64_t handle, std::size_t slotCount,
                    Memory::Vector<double> &out);

/**
 * @brief 将运行时列表转换为原生值数组。
 * @param handle 待操作的运行时句柄。
 * @param out 接收处理结果的输出对象。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool readList(stackvm::NativeCallContext &host, std::int64_t handle,
              Memory::Vector<NativeValue> &out);

/**
 * @brief 创建原生调用可用的运行时映射。
 * @retval 创建原生调用可用的运行时映射。
 * @param host 当前调用所属的公开运行时上下文。
 */
std::int64_t makeMap(stackvm::NativeCallContext &host);

/**
 * @brief 创建原生调用可用的运行时集合。
 * @retval 创建原生调用可用的运行时集合。
 * @param host 当前调用所属的公开运行时上下文。
 */
std::int64_t makeSet(stackvm::NativeCallContext &host);

/**
 * @brief 向运行时映射写入一个键值对。
 * @param mapHandle 目标运行时映射的句柄。
 * @param key 查找或写入使用的键。
 * @param value 待转换、保存或参与计算的值。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool mapSet(stackvm::NativeCallContext &host, std::int64_t mapHandle, const NativeValue &key,
            const NativeValue &value);

/**
 * @brief 将原生值插入运行时集合。
 * @param setHandle 目标运行时集合的句柄。
 * @param value 待转换、保存或参与计算的值。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool setInsert(stackvm::NativeCallContext &host, std::int64_t setHandle, const NativeValue &value);

/**
 * @brief 读取运行时映射的全部键值。
 * @param handle 待操作的运行时句柄。
 * @param keys 接收映射键的输出列表。
 * @param values 输入值列表或接收读取值的输出列表。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool readMap(stackvm::NativeCallContext &host, std::int64_t handle, Memory::Vector<NativeValue> &keys,
             Memory::Vector<NativeValue> &values);

/**
 * @brief 读取运行时集合中的原生值。
 * @param handle 待操作的运行时句柄。
 * @param elements 元素类型或元素值列表。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 * @param host 当前调用所属的公开运行时上下文。
 */
bool readSet(stackvm::NativeCallContext &host, std::int64_t handle, Memory::Vector<NativeValue> &elements);

/**
 * @brief 将集合胶水的内部槽函数绑定到公开的原生注册表。
 * @param registry 目标注册表。
 * @param error 接收注册失败原因。
 * @return 全部绑定成功时返回 true。
 */
bool bind(stackvm::NativeRegistry &registry, Memory::String *error = nullptr);
}  // namespace collection_glue
