/**
 * @file chali_abi_internal.h
 * @brief 直通 ABI 各胶水模块的内部登记表接口。
 *
 * 每个 C 胶水模块单独一个编译单元，各自导出一张按名称升序排列的入口表；
 * `chali_abi.c` 负责把各模块的表聚合成一张全局表供解析使用。
 *
 * @note 本头文件只在 source/abi 内部使用，不对宿主暴露。
 */
#ifndef CHALI_ABI_INTERNAL_H
#define CHALI_ABI_INTERNAL_H

#include "chali_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 聚合各模块入口表并排序；可重复调用，只准备一次。
 */
void chali_abi_prepare(void);

/**
 * @brief 向调用方缓冲写入有界诊断，不分配内存。
 * @param error 可选的诊断缓冲。
 * @param status 稳定状态码。
 * @param message 失败文本，成功时为空。
 * @return 输入状态码。
 */
int chali_abi_status(ChaliAbiError *error, int status, const char *message);

/**
 * @brief 取得第 `index` 个实参的值类别。
 * @param frame 当前调用帧。
 * @param index 实参下标。
 * @return `ChaliAbiSlotFloat` 或 `ChaliAbiSlotInteger`；越界时返回整数。
 */
int chali_abi_argument_kind(const ChaliAbiFrame *frame, int64_t index);

/**
 * @brief 把双精度值复制成 64 位槽的位模式。
 * @param value 双精度值。
 * @return 对应的位模式。
 */
int64_t chali_abi_slot_from_real(double value);

/**
 * @brief 取（必要时登记）某个类型名对应的宿主记录描述符。
 * @param typeName 静态类型名，不得为空。
 * @param clone 自动复制回调；为空时记录不可自动复制。
 * @param destroy 销毁回调，仅首次登记时使用。
 * @return 地址稳定的描述符；参数无效时返回空指针。
 */
const void *chali_abi_record_type(const char *typeName, void (*destroy)(void *), ChaliAbiClone clone);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHALI_ABI_INTERNAL_H
