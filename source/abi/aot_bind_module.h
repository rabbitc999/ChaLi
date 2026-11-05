/**
 * @file aot_bind_module.h
 * @brief 在模块所属编译单元中生成唯一的 AOT 注册入口。
 */
#include "chali_standard_internal.h"
#ifndef CHALI_AOT_BIND_MODULE
#error "CHALI_AOT_BIND_MODULE must name the owning module"
#endif

/**
 * @brief 提供模块局部的 C 表登记实现，未使用的静态内联实现不产生链接引用。
 * @param target 未封存的注册句柄。
 * @param failure 接收诊断的有界缓冲区。
 * @return C ABI 状态码。
 */
#define CHALI_AOT_MODULE(name, ...)                                                                                       \
    static inline int32_t chali_local_bind_##name(ChaliAbiRegistry *target, ChaliAbiError *failure)                        \
    {                                                                                                                     \
        const ChaliAbiModule modules[] = { __VA_ARGS__ };                                                                  \
        return chali_abi_register_tables(target, modules, sizeof(modules) / sizeof(modules[0]), failure);                   \
    }
#include "aot_modules.inc"
#undef CHALI_AOT_MODULE
#define CHALI_AOT_JOIN_DETAIL(left, right) left##right
#define CHALI_AOT_JOIN(left, right) CHALI_AOT_JOIN_DETAIL(left, right)
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 仅登记当前模块，应用链接不引用编译器的全量选择表。
 * @param target 未封存的注册句柄。
 * @param failure 接收诊断的有界缓冲区。
 * @return C ABI 状态码。
 */
int32_t CHALI_AOT_JOIN(chali_std_bind_, CHALI_AOT_BIND_MODULE)(ChaliAbiRegistry *target, ChaliAbiError *failure)
{
    return CHALI_AOT_JOIN(chali_local_bind_, CHALI_AOT_BIND_MODULE)(target, failure);
}
#ifdef __cplusplus
}
#endif
#undef CHALI_AOT_JOIN
#undef CHALI_AOT_JOIN_DETAIL
#undef CHALI_AOT_BIND_MODULE
