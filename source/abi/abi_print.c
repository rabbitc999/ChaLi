/**
 * @file abi_print.c
 * @brief `std::lang` 标准错误输出原语的直通 C 实现。
 *
 * eprint / eprintln 的包装逻辑已上移到 std/lang.chl（ChaLi 实现），本模块只保留
 * 唯一的流原语 `__std_stderr_write`，供 显式步数检查等未启用直连降级的路径回退使用。
 *
 * 参数文本按零拷贝视图读取：`chali_abi_bytes` 直接返回 VM 数据区或堆上的
 * 物理字节，不复制成任何宿主字符串对象。
 */
#include "chali_abi_internal.h"

/**
 * @brief 把第 0 个实参当作文本写入标准错误流。
 * @param frame 当前调用帧。
 * @return 成功返回零，读取失败报告宿主错误。
 */
static void abiStderrWrite(ChaliAbiFrame *frame)
{
    int64_t address = 0;
    chali_abi_argument_integer(frame, 0, &address);
    const unsigned char *data = NULL;
    size_t length = 0;
    if (!chali_abi_bytes(frame, address, &data, &length))
    {
        chali_abi_fail(frame, "胶水层读取 string 失败");
        chali_abi_return_integer(frame, 0);
        return;
    }
    chali_abi_emit_error(frame, (const char *)data, length, 0);
    chali_abi_return_integer(frame, 0);
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "std::lang::__std_stderr_write", abiStderrWrite, 1 },
};

const ChaliAbiRecord *abi_print_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
