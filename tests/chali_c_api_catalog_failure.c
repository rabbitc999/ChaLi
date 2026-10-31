/** @file chali_c_api_catalog_failure.c @brief 超容量内建目录必须整体失败，不能截断或越界。 */
#include "abi/chali_abi.h"
#include <assert.h>
#include <stdio.h>

/**
 * @brief 不会被调用的测试入口。
 * @param frame 未使用的帧。
 */
static void unused(ChaliAbiFrame *frame) { (void)frame; }
static const ChaliAbiRecord record = { "overflow", unused, 0 };

/**
 * @brief 报告超过目录容量的模块；实际只有一个元素，禁止越界读取。
 * @param count 接收故意超大的数量。
 * @return 唯一元素地址。
 */
const ChaliAbiRecord *abi_print_records(size_t *count)
{
    *count = 1025;
    return &record;
}
/** @brief 生成不参与此目录的空模块访问器。
 * @param name 访问器名称。
 */
#define EMPTY_MODULE(name)                    \
    const ChaliAbiRecord *name(size_t *count) \
    {                                         \
        *count = 0;                           \
        return NULL;                          \
    }
EMPTY_MODULE(abi_string_records)
EMPTY_MODULE(abi_math_records)
EMPTY_MODULE(abi_clock_records)
EMPTY_MODULE(abi_random_records)
EMPTY_MODULE(abi_parse_records)
EMPTY_MODULE(abi_process_records)
EMPTY_MODULE(abi_compress_records)
EMPTY_MODULE(abi_json_records)
EMPTY_MODULE(abi_toml_records)
EMPTY_MODULE(abi_file_records)
EMPTY_MODULE(abi_scalar_records)
EMPTY_MODULE(abi_filesystem_records)
EMPTY_MODULE(abi_regex_records)
EMPTY_MODULE(abi_serialization_records)
/**
 * @brief 空错误服务，仅用于连接未调用的参数访问器。
 * @param frame 未使用的帧。
 * @param message 未使用的消息。
 */
void chali_abi_fail(const ChaliAbiFrame *frame, const char *message)
{
    (void)frame;
    (void)message;
}
/**
 * @brief 连续两次核对失败的目录，不能发布部分数据或停留在构建状态。
 * @return 全部通过为零。
 */
int main(void)
{
    for (int at = 0; at < 2; ++at)
    {
        size_t count = 99;
        assert(chali_abi_records(&count) == NULL && count == 0);
        assert(chali_abi_resolve("overflow", NULL) == NULL);
    }
    puts("oversized C catalog rejected atomically");
    return 0;
}
