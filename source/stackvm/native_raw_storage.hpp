/**
 * @file native_raw_storage.hpp
 * @brief 原始存储的受检分配、类型身份和初始化状态入口。
 */
#pragma once
#include "runtime_objects.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 处理原始存储与裸字节操作；裸字节路径不检查元素类型或改变初始化位图。
 * @param ctx 活跃执行上下文。
 * @param args 操作及五个操作参数；字节操作使用目标地址、源地址或填充字节及字节数。
 * @return 操作结果；失败记录诊断并返回零。
 */
Slot rawStorage(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 供 VM 和 AOT 直接调用的原始存储标量内核，避免参数向量和注册表分派。
 * @param ctx 活跃执行上下文。
 * @param op 操作编号。
 * @param handle 分配句柄或目标地址。
 * @param index 元素下标、源地址或填充值。
 * @param value 操作值或字节数。
 * @param alignment 分配对齐。
 * @param tag 受检槽位类型身份，裸字节操作忽略。
 * @return 操作结果；错误保存在上下文中。
 */
Slot rawStorageOperation(NativeContext &ctx, std::int64_t op, std::int64_t handle, std::int64_t index,
                         std::int64_t value, std::int64_t alignment, std::int64_t tag);

/**
 * @brief 验证完整字节范围并借出物理地址，不读写类型或初始化状态，也不修改目标。
 * @param ctx 活跃数据区与堆。
 * @param operation 7 表示非重叠复制，8 表示填充。
 * @param destination 目标逻辑地址。
 * @param sourceOrByte 源逻辑地址或填充字节。
 * @param bytes 字节数，零长度允许空地址。
 * @param output 接收目标物理地址，零长度为空。
 * @param input 接收源物理地址，填充或零长度为空。
 * @return 验证成功为真；失败保留诊断，目标字节不变。
 */
bool rawMemoryView(NativeContext &ctx, std::int64_t operation, std::int64_t destination,
                   std::int64_t sourceOrByte, std::int64_t bytes, unsigned char *&output, unsigned char *&input);
}  // namespace stackvm::runtimeDetail
