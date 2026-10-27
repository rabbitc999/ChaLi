#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32)
#define CHALI_ABI_EXPORT __declspec(dllexport)
#else
#define CHALI_ABI_EXPORT
#endif

/* 每个入口只接收标量或函数指针：ChaLi 侧把函数值当机器函数指针传进来，
 * 宿主直接调用；捕获环境由 ChaLi 运行时随函数值一起带过来，宿主不解释它。 */
CHALI_ABI_EXPORT int64_t hostApply(int64_t (*function)(int64_t), int64_t value)
{
    return function(value);
}

CHALI_ABI_EXPORT float hostFloat(float (*function)(float), float value)
{
    return function(value);
}

CHALI_ABI_EXPORT bool hostBool(bool (*function)(bool), bool value)
{
    return function(value);
}

CHALI_ABI_EXPORT void hostNotify(void (*function)(int64_t), int64_t value)
{
    function(value);
}

/* C 的 `int64_t *` 对应 ChaLi 的 `int64 mut&`，调用处写 `&counter`。 */
CHALI_ABI_EXPORT void hostUpdate(int64_t (*function)(int64_t), int64_t *value)
{
    *value = function(*value);
}
