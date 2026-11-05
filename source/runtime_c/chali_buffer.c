/**
 * @file chali_buffer.c
 * @brief 具有失败原子性的 C 连续存储。
 */
#include "chali_buffer.h"
#include <stdint.h>
#include <string.h>

/**
 * @brief 检查缓冲区元素宽度与分配接口。
 * @param buffer 缓冲区。
 * @return 有效时返回一。
 */
static int valid_buffer(const ChaliCBuffer *buffer)
{
    return buffer && buffer->element_size && chali_c_allocator_valid(&buffer->allocator);
}
/**
 * @brief 按有效长度计算追加后的增长容量。
 * @param buffer 缓冲区。
 * @param needed 所需长度。
 * @return 合法的最小增长容量。
 */
static size_t growth_capacity(const ChaliCBuffer *buffer, size_t needed)
{
    if (needed <= buffer->capacity) return buffer->capacity;
    const size_t maximum = SIZE_MAX / buffer->element_size;
    if (buffer->length > maximum / 2) return needed;
    const size_t doubled = buffer->length * 2;
    return doubled > needed ? doubled : needed;
}
/**
 * @brief 检查源是否为自身有效区域，并给出字节偏移。
 * @param buffer 缓冲区。
 * @param values 源元素。
 * @param bytes 源字节数。
 * @param offset 接收偏移。
 * @return 外部源返回零，合法内部源返回一，内部范围无效返回负一。
 */
static int source_offset(const ChaliCBuffer *buffer, const void *values, size_t bytes, size_t *offset)
{
    if (!buffer->data || !values) return 0;
    const uintptr_t begin = (uintptr_t)buffer->data;
    const uintptr_t source = (uintptr_t)values;
    const size_t allocated = buffer->capacity * buffer->element_size;
    if (source < begin || source - begin >= allocated) return 0;
    const size_t at = (size_t)(source - begin);
    const size_t used = buffer->length * buffer->element_size;
    if (at > used || bytes > used - at) return -1;
    *offset = at;
    return 1;
}
/**
 * @brief 建立有明确元素宽度的空缓冲区。
 * @param buffer 未持有存储的目标。
 * @param element_size 元素宽度。
 * @param allocator 可选分配接口。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_init(ChaliCBuffer *buffer, size_t element_size, const ChaliCAllocator *allocator)
{
    if (!buffer || !element_size || (allocator && !chali_c_allocator_valid(allocator)))
        return CHALI_C_INVALID_ARGUMENT;
    buffer->data = NULL;
    buffer->length = 0;
    buffer->capacity = 0;
    buffer->element_size = element_size;
    buffer->allocator = allocator ? *allocator : chali_c_default_allocator();
    return CHALI_C_OK;
}
/**
 * @brief 回收独占存储。
 * @param buffer 已初始化缓冲区。
 */
void chali_c_buffer_clear(ChaliCBuffer *buffer)
{
    if (!valid_buffer(buffer)) return;
    if (buffer->data)
        buffer->allocator.release(buffer->allocator.context, buffer->data, buffer->capacity * buffer->element_size);
    buffer->data = NULL;
    buffer->length = 0;
    buffer->capacity = 0;
}
/**
 * @brief 先分配和复制，再发布新容量。
 * @param buffer 已初始化缓冲区。
 * @param capacity 最低容量。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_reserve(ChaliCBuffer *buffer, size_t capacity)
{
    if (!valid_buffer(buffer)) return CHALI_C_INVALID_ARGUMENT;
    if (capacity <= buffer->capacity) return CHALI_C_OK;
    const size_t maximum = SIZE_MAX / buffer->element_size;
    if (capacity > maximum) return CHALI_C_RANGE_ERROR;
    void *memory = buffer->allocator.allocate(buffer->allocator.context, capacity * buffer->element_size);
    if (!memory) return CHALI_C_OUT_OF_MEMORY;
    if (buffer->length) memcpy(memory, buffer->data, buffer->length * buffer->element_size);
    if (buffer->data)
        buffer->allocator.release(buffer->allocator.context, buffer->data, buffer->capacity * buffer->element_size);
    buffer->data = memory;
    buffer->capacity = capacity;
    return CHALI_C_OK;
}
/**
 * @brief 保留已有元素并将新增元素清零。
 * @param buffer 已初始化缓冲区。
 * @param length 新长度。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_resize(ChaliCBuffer *buffer, size_t length)
{
    if (!valid_buffer(buffer)) return CHALI_C_INVALID_ARGUMENT;
    const ChaliCStatus status = chali_c_buffer_reserve(buffer, growth_capacity(buffer, length));
    if (status != CHALI_C_OK) return status;
    if (length > buffer->length)
        memset((unsigned char *)buffer->data + buffer->length * buffer->element_size,
               0, (length - buffer->length) * buffer->element_size);
    buffer->length = length;
    return CHALI_C_OK;
}
/**
 * @brief 在临时存储中完成复制后替换目标。
 * @param target 独立目标。
 * @param source 源。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_copy(ChaliCBuffer *target, const ChaliCBuffer *source)
{
    if (!valid_buffer(target) || !valid_buffer(source) || target->element_size != source->element_size)
        return CHALI_C_INVALID_ARGUMENT;
    if (target == source) return CHALI_C_OK;
    ChaliCBuffer copy;
    chali_c_buffer_init(&copy, target->element_size, &target->allocator);
    const ChaliCStatus status = chali_c_buffer_append(&copy, source->data, source->length);
    if (status != CHALI_C_OK) { chali_c_buffer_clear(&copy); return status; }
    chali_c_buffer_clear(target);
    *target = copy;
    return CHALI_C_OK;
}
/**
 * @brief 在末尾追加并在扩容后重新定位内部源。
 * @param buffer 目标。
 * @param values 源。
 * @param count 元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_append(ChaliCBuffer *buffer, const void *values, size_t count)
{
    if (!valid_buffer(buffer) || (count && !values)) return CHALI_C_INVALID_ARGUMENT;
    if (!count) return CHALI_C_OK;
    if (count > SIZE_MAX / buffer->element_size || count > SIZE_MAX - buffer->length)
        return CHALI_C_RANGE_ERROR;
    const size_t bytes = count * buffer->element_size;
    size_t offset = 0;
    const int internal = source_offset(buffer, values, bytes, &offset);
    if (internal < 0) return CHALI_C_RANGE_ERROR;
    const ChaliCStatus status = chali_c_buffer_reserve(buffer, growth_capacity(buffer, buffer->length + count));
    if (status != CHALI_C_OK) return status;
    if (internal) values = (const unsigned char *)buffer->data + offset;
    memmove((unsigned char *)buffer->data + buffer->length * buffer->element_size, values, bytes);
    buffer->length += count;
    return CHALI_C_OK;
}
/**
 * @brief 先保存内部源，再移动尾部并写入新元素。
 * @param buffer 目标。
 * @param index 插入位置。
 * @param values 源。
 * @param count 元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_insert(ChaliCBuffer *buffer, size_t index, const void *values, size_t count)
{
    if (!valid_buffer(buffer) || index > buffer->length || (count && !values)) return CHALI_C_INVALID_ARGUMENT;
    if (!count) return CHALI_C_OK;
    if (count > SIZE_MAX / buffer->element_size || count > SIZE_MAX - buffer->length)
        return CHALI_C_RANGE_ERROR;
    const size_t bytes = count * buffer->element_size;
    size_t offset = 0;
    const int internal = source_offset(buffer, values, bytes, &offset);
    if (internal < 0) return CHALI_C_RANGE_ERROR;
    void *saved = NULL;
    if (internal)
    {
        saved = buffer->allocator.allocate(buffer->allocator.context, bytes);
        if (!saved) return CHALI_C_OUT_OF_MEMORY;
        memcpy(saved, values, bytes);
        values = saved;
    }
    const ChaliCStatus status = chali_c_buffer_reserve(buffer, growth_capacity(buffer, buffer->length + count));
    if (status == CHALI_C_OK)
    {
        unsigned char *at = (unsigned char *)buffer->data + index * buffer->element_size;
        memmove(at + bytes, at, (buffer->length - index) * buffer->element_size);
        memcpy(at, values, bytes);
        buffer->length += count;
    }
    if (saved) buffer->allocator.release(buffer->allocator.context, saved, bytes);
    return status;
}
/**
 * @brief 有界移除元素。
 * @param buffer 目标。
 * @param index 起始下标。
 * @param count 元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_erase(ChaliCBuffer *buffer, size_t index, size_t count)
{
    if (!valid_buffer(buffer) || index > buffer->length || count > buffer->length - index)
        return CHALI_C_INVALID_ARGUMENT;
    if (count)
        memmove((unsigned char *)buffer->data + index * buffer->element_size,
                (unsigned char *)buffer->data + (index + count) * buffer->element_size,
                (buffer->length - index - count) * buffer->element_size);
    buffer->length -= count;
    return CHALI_C_OK;
}
