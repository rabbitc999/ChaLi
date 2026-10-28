/**
 * @file native_text.hpp
 * @brief VM 内部 native text 模块的类型与接口。
 */
#pragma once
#include "runtime_objects.hpp"
#include "value_transfer.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 按标量协议查询字符串长度、数据地址、容量或描述符标记。
 * @param ctx 当前地址空间及诊断上下文。
 * @param operation 零为长度，一为数据，二为容量，三为描述符标记。
 * @param address 字符串逻辑地址。
 * @return 对应元数据；失败保留 ctx.error。
 */
std::int64_t stringStorageQuery(NativeContext &ctx, std::int64_t operation, std::int64_t address);
/**
 * @brief 查询字符串字节容量，不包含终止字节。
 * @param ctx 当前调用上下文。
 * @param args 唯一参数为字符串句柄。
 * @return 容量整数槽。
 */
Slot strCapacity(NativeContext &ctx, const Memory::Vector<Slot> &args);
/**
 * @brief 判断句柄是否使用可增长容量描述符。
 * @param ctx 当前调用上下文。
 * @param args 唯一参数为字符串句柄。
 * @return 描述符为一，其他存储为零。
 */
Slot strStorageKind(NativeContext &ctx, const Memory::Vector<Slot> &args);
/**
 * @brief 查询默认字节堆捕获的分配器预算。
 * @param ctx 当前调用上下文。
 * @param args 无参数。
 * @return 最大单块载荷预算，未知堆报告不支持。
 */
Slot bufferMaxBytes(NativeContext &ctx, const Memory::Vector<Slot> &args);
/**
 * @brief `__std_str_len(地址) -> 长度`。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strLen(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 将字符串值转换为首个内容字节地址，区分长度头与字面量。
 * @param ctx 当前内存及分配身份上下文。
 * @param args 唯一参数为字符串值的逻辑地址。
 * @return 首个内容字节地址；空值为零。
 */
Slot strData(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief `__std_str_fmt(格式串, 实参…) -> 新串地址`。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strFormat(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 输出一段文本：挂了下沉就先实时推给宿主，`keepText=false` 时不再进文本桶。
 * @param ctx 环境。
 * @param bytes 字节。
 * @param count 字节数。
 */
void nativeEmitText(NativeContext &ctx, const char *bytes, std::size_t count);

/**
 * @brief `__std_print(格式串, 实参…) -> 0`：把格式化结果追加到文本输出桶（`printf` 用它）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strPrint(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 输出一个字符。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot putChar(NativeContext &ctx, const Memory::Vector<Slot> &args);

}  // namespace stackvm::runtimeDetail
