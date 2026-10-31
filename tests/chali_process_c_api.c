/** @file chali_process_c_api.c @brief 进程 C 帧的确定性生命周期与失败清理单测。 */
#include "abi/chali_standard.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int opened, closed, failed, adoptionFailure, openFailure;
static void *record;
static ChaliAbiDestroy destroyRecord;
static const char *command = "echo unit";
static char message[256], returned[1024];
static const char *streamPath;

/**
 * @brief 用真实临时流模拟子进程输出，避免依赖 shell 调度。
 * @param text 必须完整复制的命令。
 * @param mode 读取模式。
 * @return 测试流，注入失败时为空。
 */
static FILE *mockOpen(const char *text, const char *mode)
{
    assert(strcmp(text, command) == 0 && strcmp(mode, "r") == 0);
    if (openFailure) return NULL;
    FILE *file = fopen(streamPath, "w+b");
    assert(file);
    for (int index = 0; index < 1024; ++index) fputc('x', file);
    fputs("\r\n", file);
    rewind(file);
    ++opened;
    return file;
}
/**
 * @brief 关闭模拟管道并记录精确次数。
 * @param file 存活的测试流。
 * @return 固定退出状态。
 */
static int mockClose(FILE *file)
{
    assert(fclose(file) == 0);
    assert(remove(streamPath) == 0);
    ++closed;
    return 17;
}
/**
 * @brief 核对命令文本并模拟负退出状态。
 * @param text 必须在调用期间存活的命令。
 * @return 固定负状态。
 */
static int mockSystem(const char *text)
{
    assert(strcmp(text, command) == 0);
    return -7;
}

static int allocations, failAt, liveAllocations;
/**
 * @brief 对生产 C 分配点逐次注入失败并统计实际存活块。
 * @param size 请求字节数。
 * @return 分配的块，命中失败点时为空。
 */
static void *mockMalloc(size_t size)
{
    ++allocations;
    if (failAt && allocations == failAt) return NULL;
    void *value = malloc(size);
    assert(value);
    ++liveAllocations;
    return value;
}
/**
 * @brief 释放生产入口创建的块并检查计数。
 * @param value 可为空的分配块。
 */
static void mockFree(void *value)
{
    if (value)
    {
        assert(liveAllocations > 0);
        --liveAllocations;
    }
    free(value);
}
#define malloc mockMalloc
#define free   mockFree
#ifdef _WIN32
#define _popen  mockOpen
#define _pclose mockClose
#else
#define popen  mockOpen
#define pclose mockClose
#endif
#define system mockSystem
#include "../source/abi/abi_process.c"
#undef malloc
#undef free
#undef system
#ifdef _WIN32
#undef _popen
#undef _pclose
#else
#undef popen
#undef pclose
#endif

/**
 * @brief 记录 C 边界报告的首个错误。
 * @param frame 本测试不需要运行时实例。
 * @param text 错误文本。
 */
void CHALI_ABI_CALL chali_abi_fail(const ChaliAbiFrame *frame, const char *text)
{
    (void)frame;
    if (!failed)
    {
        snprintf(message, sizeof(message), "%s", text);
        failed = 1;
    }
}
/**
 * @brief 核对帧的整数输入。
 * @param frame 当前帧。
 * @param index 参数下标。
 * @param out 成功时接收参数。
 * @return 输入有效时为一。
 */
int CHALI_ABI_CALL chali_abi_argument_integer(const ChaliAbiFrame *frame, int64_t index, int64_t *out)
{
    if (!frame || index >= frame->count || !frame->slots || !frame->kinds || frame->kinds[index] != ChaliAbiSlotInteger)
    {
        chali_abi_fail(frame, "invalid argument");
        return 0;
    }
    *out = frame->slots[index];
    return 1;
}
/**
 * @brief 借用测试命令的原始字节。
 * @param frame 当前帧。
 * @param address 测试字符串句柄。
 * @param data 接收借用指针。
 * @param length 接收精确长度。
 * @return 有效时为一。
 */
int CHALI_ABI_CALL chali_abi_bytes(const ChaliAbiFrame *frame, int64_t address, const unsigned char **data, size_t *length)
{
    if (address != 1)
    {
        chali_abi_fail(frame, "invalid string");
        return 0;
    }
    *data = (const unsigned char *)command;
    *length = strlen(command);
    return 1;
}
/**
 * @brief 复制结果字节，模拟拥有型字符串创建。
 * @param frame 当前帧。
 * @param data 待复制字节。
 * @param length 字节数。
 * @return 固定结果句柄。
 */
int64_t CHALI_ABI_CALL chali_abi_string(const ChaliAbiFrame *frame, const char *data, size_t length)
{
    (void)frame;
    assert(length < sizeof(returned));
    memcpy(returned, data, length);
    returned[length] = 0;
    return 2;
}
/**
 * @brief 写回整数结果槽。
 * @param frame 当前帧。
 * @param value 整数结果。
 */
void CHALI_ABI_CALL chali_abi_return_integer(ChaliAbiFrame *frame, int64_t value)
{
    frame->result = value;
    frame->resultIsFloat = 0;
}
/**
 * @brief 仅在成功时接管记录，失败时保留调用方所有权。
 * @param frame 当前帧。
 * @param type 规范类型名。
 * @param value 待接管对象。
 * @param destroy 销毁回调。
 * @return 接管成功时的测试句柄，失败为零。
 */
int64_t CHALI_ABI_CALL chali_abi_record_make(const ChaliAbiFrame *frame, const char *type, void *value, ChaliAbiDestroy destroy)
{
    assert(strcmp(type, "std::process::Process") == 0 && !record);
    if (adoptionFailure)
    {
        chali_abi_fail(frame, "injected adoption failure");
        return 0;
    }
    record = value;
    destroyRecord = destroy;
    return 9;
}
/**
 * @brief 只借用本测试登记的 Process。
 * @param frame 当前帧。
 * @param handle 记录句柄。
 * @param type 规范类型名。
 * @return 有效记录指针。
 */
void *CHALI_ABI_CALL chali_abi_record_borrow(const ChaliAbiFrame *frame, int64_t handle, const char *type)
{
    assert(strcmp(type, "std::process::Process") == 0);
    if (handle != 9 || !record)
    {
        chali_abi_fail(frame, "invalid record");
        return NULL;
    }
    return record;
}
/**
 * @brief 销毁已接管的记录一次。
 * @param frame 当前帧。
 * @param handle 待释放句柄。
 * @return 成功释放时为一。
 */
int CHALI_ABI_CALL chali_abi_record_release(const ChaliAbiFrame *frame, int64_t handle)
{
    if (handle != 9 || !record)
    {
        chali_abi_fail(frame, "invalid release");
        return 0;
    }
    void *value = record;
    record = NULL;
    destroyRecord(value);
    return 1;
}
/**
 * @brief 查找实际生产入口。
 * @param name 完整入口名。
 * @return 实际 C 帧函数。
 */
static ChaliAbiEntry entry(const char *name)
{
    size_t count = 0;
    const ChaliAbiRecord *table = abi_process_records(&count);
    assert(count == 5);
    for (size_t index = 0; index < count; ++index)
        if (!strcmp(table[index].name, name)) return table[index].entry;
    abort();
}
/** @brief 验证分段、关闭、自动释放与接管失败清理。
 * @param argc 必须提供一个工作目录中的流文件路径。
 * @param argv 参数数组，第二项为本测试独占的临时流路径。
 * @return 全部通过时为零。
 */
int main(int argc, char **argv)
{
    assert(argc == 2);
    streamPath = argv[1];
    int64_t argument = 1;
    unsigned char kind = ChaliAbiSlotInteger;
    ChaliAbiFrame frame = { NULL, &argument, &kind, 1, 0, 0 };
    entry("std::process::__processRun")(&frame);
    assert(frame.result == 9 && opened == 1 && !failed);
    argument = 9;
    for (int index = 0; index < 4; ++index)
    {
        entry("std::process::__processRead")(&frame);
        assert(!failed && frame.result == 2 && strlen(returned) == (index < 2 ? 511u : index == 2 ? 2u :
                                                                                                    0u));
    }
    entry("std::process::__processClose")(&frame);
    assert(frame.result == 17 && closed == 1);
    entry("std::process::__processRead")(&frame);
    assert(failed && !strcmp(message, "pread 用到的进程句柄无效或已经关闭"));
    failed = 0;
    entry("std::process::__processClose")(&frame);
    assert(failed && !strcmp(message, "pclose 用到的进程句柄无效或已经关闭"));
    failed = 0;
    entry("std::process::__processDrop")(&frame);
    assert(!record && closed == 1 && !failed);
    argument = 1;
    entry("std::process::__processRun")(&frame);
    argument = 9;
    entry("std::process::__processDrop")(&frame);
    assert(opened == 2 && closed == 2 && !record);
    argument = 1;
    adoptionFailure = 1;
    entry("std::process::__processRun")(&frame);
    assert(failed && frame.result == 0 && opened == 3 && closed == 3 && !record);
    failed = 0;
    adoptionFailure = 0;
    openFailure = 1;
    entry("std::process::__processRun")(&frame);
    assert(failed && strstr(message, command) && opened == closed);
    failed = 0;
    openFailure = 0;
    allocations = 0;
    failAt = 1;
    entry("std::process::__processRun")(&frame);
    assert(failed && !strcmp(message, "命令文本分配失败") && opened == closed && !record && !liveAllocations);
    failed = 0;
    allocations = 0;
    failAt = 2;
    entry("std::process::__processRun")(&frame);
    assert(failed && !strcmp(message, "进程记录分配失败") && opened == closed && !record && !liveAllocations);
    failed = 0;
    allocations = 0;
    failAt = 2;
    openFailure = 1;
    entry("std::process::__processRun")(&frame);
    assert(failed && !strcmp(message, "启动命令失败：") && opened == closed && !record && !liveAllocations);
    failed = 0;
    failAt = 0;
    openFailure = 0;
    entry("std::process::__system")(&frame);
    assert(!failed && frame.result == -7);
    kind = ChaliAbiSlotFloat;
    entry("std::process::__processRun")(&frame);
    assert(failed && opened == closed);
    assert(!liveAllocations && !record && opened == closed);
    puts("C process: chunks, close/drop once, command/record/error allocation failures, adoption cleanup and slot checks passed");
    return 0;
}
