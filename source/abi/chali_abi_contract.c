/**
 * @file chali_abi_contract.c
 * @brief 无分配的 C ABI 目标、模块和调用帧契约检查。
 */
#include "chali_abi_internal.h"

#include <string.h>

_Static_assert(sizeof(double) == sizeof(int64_t), "C ABI requires 64-bit double slots");

/**
 * @brief 写入有界诊断并返回状态；首次或复用缓冲均可。
 * @param error 调用方缓冲，可为空。
 * @param status 本次状态。
 * @param message 失败诊断；成功时为空。
 * @return 输入状态。
 */
int chali_abi_status(ChaliAbiError *error, int status, const char *message)
{
    if (error != NULL)
    {
        const size_t length = message == NULL ? 0u : strlen(message);
        error->status = status;
        error->required = status == ChaliAbiOk ? 0u : length + 1u;
        if (error->data != NULL && error->capacity != 0u)
        {
            const size_t count = length < error->capacity ? length : error->capacity - 1u;
            if (count != 0u) memcpy(error->data, message, count);
            error->data[count] = '\0';
        }
    }
    return status;
}

/**
 * @brief 返回静态只读的宿主布局；查询不写共享数据。
 * @return 当前目标描述的进程级地址。
 */
const ChaliAbiInfo *chali_abi_info(void)
{
    /* 按常量初始化，避免并发首次查询时写共享存储。 */
    static const ChaliAbiInfo info = {
        sizeof(ChaliAbiInfo), CHALI_ABI_VERSION, sizeof(void *) * 8u,
#if defined(_WIN32) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        1,
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        0,
#else
#error "请为此目标定义字节序后构建 C ABI"
#endif
        CHALI_ABI_TARGET_CONVENTION,
        CHALI_ABI_TARGET_ARCH,
        CHALI_ABI_TARGET_PLATFORM,
        sizeof(ChaliAbiFrame),
        _Alignof(ChaliAbiFrame),
        sizeof(ChaliAbiRecord),
        _Alignof(ChaliAbiRecord),
        { offsetof(ChaliAbiFrame, runtime), offsetof(ChaliAbiFrame, slots), offsetof(ChaliAbiFrame, kinds), offsetof(ChaliAbiFrame, count), offsetof(ChaliAbiFrame, result), offsetof(ChaliAbiFrame, resultIsFloat) },
        { offsetof(ChaliAbiRecord, name), offsetof(ChaliAbiRecord, entry), offsetof(ChaliAbiRecord, arity) },
        CHALI_ABI_CAP_SYNC_CALLBACK | CHALI_ABI_CAP_RECORD | CHALI_ABI_CAP_BYTE_VIEW | CHALI_ABI_CAP_REGISTRY | CHALI_ABI_CAP_MEMORY
    };
    return &info;
}

/**
 * @brief 逐字段比较模块与宿主布局及所需能力。
 * @param expected 模块本地生成的布局。
 * @param error 调用方有界诊断缓冲，可为空。
 * @return 检查状态，零表示兼容。
 */
int chali_abi_check(const ChaliAbiInfo *expected, ChaliAbiError *error)
{
    const ChaliAbiInfo *host = chali_abi_info();
    if (expected == NULL) return chali_abi_status(error, ChaliAbiInvalidArgument, "missing ABI descriptor");
    /* 先核对大小；短描述不能读取后续字段。 */
    if (expected->structSize != sizeof(ChaliAbiInfo))
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI descriptor size mismatch");
    if (expected->version != host->version)
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI version mismatch");
    if (host->architecture == 0 || host->platform == 0 || expected->architecture != host->architecture || expected->platform != host->platform)
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI architecture or platform mismatch");
    if (expected->pointerBits != host->pointerBits || expected->littleEndian != host->littleEndian)
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI target layout mismatch");
    if (expected->callingConvention != host->callingConvention)
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI calling convention mismatch");
    if (expected->frameSize != host->frameSize || expected->frameAlignment != host->frameAlignment || expected->recordSize != host->recordSize || expected->recordAlignment != host->recordAlignment || memcmp(expected->frameOffsets, host->frameOffsets, sizeof(host->frameOffsets)) != 0 || memcmp(expected->recordOffsets, host->recordOffsets, sizeof(host->recordOffsets)) != 0)
        return chali_abi_status(error, ChaliAbiIncompatible, "ABI frame or record layout mismatch");
    if ((expected->capabilities & ~host->capabilities) != 0u)
        return chali_abi_status(error, ChaliAbiIncompatible, "unsupported ABI capability");
    return chali_abi_status(error, ChaliAbiOk, NULL);
}

/**
 * @brief 无副作用检查模块全部入口及模块内重名。
 * @param module 调用方提供的模块描述。
 * @param error 调用方有界诊断缓冲，可为空。
 * @return 检查状态，零表示可进入后续注册。
 */
int chali_abi_check_module(const ChaliAbiModuleDescriptor *module, ChaliAbiError *error)
{
    size_t at;
    int status;
    if (module == NULL || module->structSize != sizeof(ChaliAbiModuleDescriptor))
        return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid module descriptor");
    status = chali_abi_check(module->abi, error);
    if (status != ChaliAbiOk) return status;
    if (module->count > SIZE_MAX / sizeof(ChaliAbiRecord) || (module->count != 0u && module->records == NULL))
        return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid module record array");
    for (at = 0; at < module->count; ++at)
    {
        size_t previous;
        const ChaliAbiRecord *record = &module->records[at];
        if (record->name == NULL || record->name[0] == '\0' || record->entry == NULL || record->arity < -1)
            return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid module entry");
        for (previous = 0; previous < at; ++previous)
            if (strcmp(module->records[previous].name, record->name) == 0)
                return chali_abi_status(error, ChaliAbiDuplicate, "duplicate module entry");
    }
    return chali_abi_status(error, ChaliAbiOk, NULL);
}

/**
 * @brief 核对实参数量、数组及每个槽的类别。
 * @param frame 调用方保证指向有效存储的帧。
 * @param arity 固定参数数量，-1 表示变参。
 * @param error 调用方有界诊断缓冲，可为空。
 * @return 检查状态，零表示输入合法。
 */
int chali_abi_check_frame(const ChaliAbiFrame *frame, int arity, ChaliAbiError *error)
{
    int64_t at;
    if (frame == NULL || arity < -1 || frame->count < 0 || (uint64_t)frame->count > SIZE_MAX / sizeof(int64_t))
        return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid call frame");
    if (arity >= 0 && frame->count != arity)
        return chali_abi_status(error, ChaliAbiInvalidArgument, "argument count mismatch");
    if (frame->count != 0 && (frame->slots == NULL || frame->kinds == NULL))
        return chali_abi_status(error, ChaliAbiInvalidArgument, "missing argument array");
    for (at = 0; at < frame->count; ++at)
        if (frame->kinds[at] != ChaliAbiSlotInteger && frame->kinds[at] != ChaliAbiSlotFloat)
            return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid argument slot kind");
    return chali_abi_status(error, ChaliAbiOk, NULL);
}

/**
 * @brief 拒绝零与一之外的返回槽类别。
 * @param frame 调用完成后的帧。
 * @param error 调用方有界诊断缓冲，可为空。
 * @return 检查状态，零表示返回类别合法。
 */
int chali_abi_check_result(const ChaliAbiFrame *frame, ChaliAbiError *error)
{
    if (frame == NULL || (frame->resultIsFloat != 0 && frame->resultIsFloat != 1))
        return chali_abi_status(error, ChaliAbiInvalidArgument, "invalid result slot kind");
    return chali_abi_status(error, ChaliAbiOk, NULL);
}
