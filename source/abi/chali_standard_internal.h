/** @file chali_standard_internal.h @brief C 模块取表及登记的共享实现。 */
#ifndef CHALI_STANDARD_INTERNAL_H
#define CHALI_STANDARD_INTERNAL_H
#include "chali_standard.h"
#include "chali_abi_internal.h"

/**
 * @brief 按声明顺序登记模块入口表，在首个失败处停止。
 * @param registry 未封存的 C 注册句柄。
 * @param modules 非空的模块取表函数数组。
 * @param count 数组元素数量。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；每张入口表的登记各自具有原子性。
 */
static inline int32_t chali_abi_register_tables(ChaliAbiRegistry *registry, const ChaliAbiModule *modules,
                                                size_t count, ChaliAbiError *error)
{
    size_t index;
    if (!registry || !modules || !count)
        return chali_abi_status(error, ChaliAbiInvalidArgument, "missing registry or module tables");
    for (index = 0; index < count; ++index)
    {
        size_t length = 0;
        const ChaliAbiRecord *records;
        ChaliAbiInfo info;
        ChaliAbiModuleDescriptor descriptor;
        int32_t status;
        if (!modules[index])
            return chali_abi_status(error, ChaliAbiInvalidArgument, "missing module table provider");
        records = modules[index](&length);
        info = chali_abi_local_info(0);
        descriptor.structSize = sizeof(descriptor);
        descriptor.abi = &info;
        descriptor.records = records;
        descriptor.count = length;
        status = chali_abi_registry_register(registry, &descriptor, error);
        if (status != ChaliAbiOk) return status;
    }
    return chali_abi_status(error, ChaliAbiOk, 0);
}
#endif
