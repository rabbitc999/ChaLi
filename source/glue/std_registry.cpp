/** @file std_registry.cpp @brief 通过 C 注册契约集中登记标准库。 */
#include "abi/chali_standard_internal.h"

/**
 * @brief 登记当前构建启用的标准库，保留首个失败诊断。
 * @param registry 未封存的 C 注册句柄。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；失败后停止登记后续模块。
 */
extern "C" int32_t chali_abi_bind_standard(ChaliAbiRegistry *registry, ChaliAbiError *error)
{
#define CHALI_STANDARD_ONLY
#define CHALI_AOT_MODULE(name, ...)                                                                                       \
    {                                                                                                                     \
        const ChaliAbiModule modules[] = { __VA_ARGS__ };                                                                 \
        const int32_t status = chali_abi_register_tables(registry, modules, sizeof(modules) / sizeof(modules[0]), error); \
        if (status != ChaliAbiOk) return status;                                                                          \
    }
#include "aot_modules.inc"
#undef CHALI_AOT_MODULE
#undef CHALI_STANDARD_ONLY
    return ChaliAbiOk;
}
