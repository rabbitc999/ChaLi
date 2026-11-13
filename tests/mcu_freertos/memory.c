#include <stddef.h>

/** @brief 填充板级内存。 */
void *memset(void *destination, int value, size_t size)
{
    unsigned char *bytes = destination;
    for (size_t i = 0; i < size; ++i) bytes[i] = (unsigned char)value;
    return destination;
}

/** @brief 复制互不重叠的板级内存。 */
void *memcpy(void *destination, const void *source, size_t size)
{
    unsigned char *out = destination;
    const unsigned char *in = source;
    for (size_t i = 0; i < size; ++i) out[i] = in[i];
    return destination;
}

/** @brief 为 FreeRTOS 复制有限长度的任务名。 */
char *strncpy(char *destination, const char *source, size_t size)
{
    size_t i = 0;
    while (i < size && source[i]) { destination[i] = source[i]; ++i; }
    while (i < size) destination[i++] = 0;
    return destination;
}
