#ifndef CHALI_MCU_MEMORY_H
#define CHALI_MCU_MEMORY_H

#include <stdint.h>

/** @brief 初始化数据映像和逻辑堆。 */
void chali_host_init(void);

/** @brief 释放模块拥有的内存。 */
void chali_host_finish(void);

/**
 * @brief 取得有效数据或堆范围的地址。
 * @param address 逻辑字节地址。
 * @param count 访问长度。
 * @return 在块释放前有效的物理地址。
 */
unsigned char *chali_host_address(int64_t address, uint64_t count);

/**
 * @brief 申请清零的逻辑堆块。
 * @param bytes 字节数。
 * @return 新块的逻辑基址。
 */
int64_t chali_host_allocate(uint64_t bytes);

/**
 * @brief 释放逻辑堆块。
 * @param address 堆块基址；零表示空句柄。
 */
void chali_host_drop(int64_t address);

#endif
