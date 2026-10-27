/**
 * @file abi_print.c
 * @brief `std::lang` 标准错误输出的直通 C 实现。
 *
 * 参数文本按零拷贝视图读取：`chali_abi_bytes` 直接返回 VM 数据区或堆上的
 * 物理字节，不复制成任何宿主字符串对象。
 */
#include "chali_abi_internal.h"

/**
 * @brief 把第 0 个实参当作文本写出。
 * @param frame 当前调用帧。
 * @param newline 是否在文本后追加换行。
 * @param errorStream 非零写标准错误流，零写标准输出桶。
 */
static void writeText(ChaliAbiFrame *frame, int newline, int errorStream)
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
    if (errorStream)
    {
        chali_abi_emit_error(frame, (const char *)data, length, newline);
    }
    else
    {
        chali_abi_emit(frame, (const char *)data, length);
        if (newline) chali_abi_emit(frame, "\n", 1u);
    }
    chali_abi_return_integer(frame, 0);
}

/**
 * @brief 写入标准错误流。
 * @param frame 帧：0 = 文本。
 */
static void abiStderrWrite(ChaliAbiFrame *frame)
{
    writeText(frame, 0, 1);
}

/**
 * @brief 写入标准错误流并换行。
 * @param frame 帧：0 = 文本。
 */
static void abiStderrWriteLine(ChaliAbiFrame *frame)
{
    writeText(frame, 1, 1);
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "std::lang::__std_eprint", abiStderrWrite, 1 },
    { "std::lang::__std_eprintln", abiStderrWriteLine, 1 },
    { "std::lang::__std_stderr_write", abiStderrWrite, 1 },
};

const ChaliAbiRecord *abi_print_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
