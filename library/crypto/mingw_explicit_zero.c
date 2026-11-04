#include <stddef.h>

/**
 * @brief 为旧 MinGW 运行库补齐锁定的 libsodium 归档所需的显式清除函数。
 * @param destination 要擦写的缓冲区。
 * @param value 填充值的低八位。
 * @param count 写入字节数。
 * @return 原缓冲区指针。
 */
void *memset_explicit(void *destination, int value, size_t count)
{
    volatile unsigned char *bytes = destination;
    while (count-- != 0) *bytes++ = (unsigned char)value;
    return destination;
}
