#pragma once
#include "stackvm.hpp"

namespace collection_glue
{
using NativeValue = stackvm::NativeValue;

/**
 * @brief 从借用字节直接构造并追加一个独立文本元素，避免通用原生值的中间副本。
 * @param host 当前运行时上下文。
 * @param listHandle List<String> 的存活句柄。
 * @param data 输入字节，空文本可为空。
 * @param length 字节数。
 * @return 追加成功为真。
 */
bool listPushText(stackvm::NativeCallContext &host, std::int64_t listHandle, const char *data, std::size_t length);

/**
 * @brief 集合原语的数值结果，直接保留位模式与种类，不构造拥有文本的通用值。
 */
struct ScalarResult
{
    std::int64_t bits = 0;       ///< 整数值或浮点数的原始位模式。
    unsigned char floating = 0;  ///< 浮点数为一，整数为零。
};

/**
 * @brief 直接复制数值位模式为集合拥有的槽，完整创建后发布句柄。
 * @param host 当前运行时上下文。
 * @param bits 数值位模式，非空跨度不能为空。
 * @param kinds 浮点为一、整数为零；空指针表示全部整数。
 * @param count 元素数量。
 * @return 独立列表句柄，失败为零。
 */
std::int64_t makeNumericList(stackvm::NativeCallContext &host, const std::int64_t *bits,
                             const unsigned char *kinds, std::size_t count);
/**
 * @brief 验证完整数值列表后复制位模式，不为没有元素视图的列表物化通用槽。
 * @param host 当前运行时上下文。
 * @param handle 数值列表句柄。
 * @param bits 接收位模式；与 kinds 同为空且 capacity 为零时仅查询。
 * @param kinds 接收整数／浮点类别。
 * @param capacity 输出元素容量。
 * @param required 接收实际元素数量。
 * @return 验证及复制成功时为真；失败不改写两个输出数组。
 */
bool copyListNumbers(stackvm::NativeCallContext &host, std::int64_t handle, std::int64_t *bits,
                      unsigned char *kinds, std::size_t capacity, std::size_t &required);
/**
 * @brief 将独立宿主数值批量追加到原列表，先验证与预留容量再写入元素。
 * @param host 当前运行时上下文。
 * @param handle 可写列表句柄。
 * @param bits 独立数值位模式数组，非空跨度不得为空。
 * @param kinds 类别数组，可为空表示全部整数。
 * @param count 追加元素数量。
 * @return 成功为真，失败不改变长度及原有元素。
 */
bool appendListNumbers(stackvm::NativeCallContext &host, std::int64_t handle, const std::int64_t *bits,
                        const unsigned char *kinds, std::size_t count);

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
 * @brief 从独立字节跨度构造拥有型整数列表，保持原有逐元素增长策略。
 * @param host 当前运行时上下文。
 * @param data 待复制字节，非空跨度不能为空。
 * @param count 字节数。
 * @return 完整创建后发布的列表句柄，失败为零。
 */
std::int64_t makeByteList(stackvm::NativeCallContext &host, const unsigned char *data, std::size_t count);

/**
 * @brief 查询或复制列表中的字节值，短缓冲和无效元素不写入目标。
 * @param host 当前运行时上下文。
 * @param handle 列表句柄。
 * @param data 可写目标；为空且 capacity 为零时仅查询。
 * @param capacity 目标字节容量。
 * @param required 接收实际字节数。
 * @return 验证及复制成功时为真。
 */
bool copyListBytes(stackvm::NativeCallContext &host, std::int64_t handle, unsigned char *data,
                   std::size_t capacity, std::size_t &required);

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
 * @brief 执行由 C 入口统一清单选择的私有集合存储操作。
 * @param host 当前对象表、内存与同步回调上下文。
 * @param operation 唯一清单中的入口下标。
 * @param bits 参数位模式。
 * @param kinds 整数或浮点种类。
 * @param count 参数数量，已由 C 入口校验。
 * @return 原始结果位模式与种类；不建立第二套注册系统。
 */
ScalarResult applyPrimitive(stackvm::NativeCallContext &host,std::size_t operation,
                            const std::int64_t *bits,const unsigned char *kinds,std::size_t count);
} // namespace collection_glue
