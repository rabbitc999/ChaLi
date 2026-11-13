#include "chali_mcu_hal.h"
#include "chali_mcu_memory.h"

extern const uint64_t chali_data_size;
extern int64_t chali_string_length(int64_t address);
extern int64_t chali_load_i64(int64_t address);
extern const char *chali_string_bytes(int64_t address);
extern _Noreturn void HAL_Chali_Panic(const char *message);

static unsigned char output_stream;
void *stdin, *stdout = &output_stream, *stderr = &output_stream;

/** @brief 将输出流的字节写入板级端口。 */
size_t fwrite(const void *data, size_t size, size_t count, void *stream)
{
    (void)stream;
    if (!size || count > SIZE_MAX / size) return 0;
    return chali_mcu_print(data, size * count) / size;
}

/** @brief 板级写入是同步操作，无需刷新。 */
int fflush(void *stream) { (void)stream; return 0; }

/**
 * @brief 报告无法继续的板级运行时错误。
 * @param message 以零结尾的错误消息。
 */
_Noreturn void chali_mcu_panic(const char *message)
{
    HAL_Chali_Panic(message);
}

/** @brief MCU 固件没有宿主命令行参数。 */
void chali_host_process_arguments(int argc, void *argv) { (void)argc; (void)argv; }

/** @brief 读取字符串长度、数据偏移、容量或存储标记。 */
int64_t chali_host_string_storage(int64_t operation, int64_t address)
{
    if (operation == 0) return chali_string_length(address);
    if (address < (int64_t)chali_data_size) return operation == 1 ? address : 0;
    const int64_t marker = chali_load_i64(address);
    if (operation == 1) return marker == -1 ? chali_load_i64(address + 24) : address + 8;
    if (operation == 2) return marker == -1 ? chali_load_i64(address + 16) : marker;
    return marker == -1 ? 2 : 1;
}

/** @brief 查询字符串属性，板级实现不缓存堆描述符。 */
int64_t chali_host_string_storage_view(int64_t operation, int64_t address, void *view)
{
    (void)view;
    return chali_host_string_storage(operation, address);
}

/** @brief 解析逻辑地址，板级实现不缓存堆块。 */
void *chali_host_address_view(int64_t address, uint64_t bytes, void *view)
{
    (void)view;
    return chali_host_address(address, bytes);
}
