/** @file main.c @brief 只包含公开 C 头的宿主注册、调用与释放示例。 */
#include "chali_abi.h"
#include <stdio.h>
#include <string.h>

static int entered = 0;

/**
 * @brief 把两个已检查的整数槽相加。
 * @param frame 本次同步调用帧。
 */
static void CHALI_ABI_CALL add(ChaliAbiFrame *frame)
{
    int64_t left = 0, right = 0;
    if (!chali_abi_argument_integer(frame, 0, &left) || !chali_abi_argument_integer(frame, 1, &right)) return;
    ++entered;
    chali_abi_return_integer(frame, left + right);
}

/**
 * @brief 报告 C 宿主失败；部分输出不能作为成功结果返回。
 * @param frame 本次同步调用帧。
 */
static void CHALI_ABI_CALL fail(ChaliAbiFrame *frame)
{
    chali_abi_return_integer(frame, 123);
    chali_abi_fail(frame, "C host requested failure");
}

/**
 * @brief 故意给出非法返回类别，用于展示边界拒绝。
 * @param frame 本次同步调用帧。
 */
static void CHALI_ABI_CALL invalid_result(ChaliAbiFrame *frame)
{
    frame->result = 123;
    frame->resultIsFloat = 2;
}

/**
 * @brief 运行独立 C 宿主示例并销毁注册表。
 * @return 所有可观察行为符合协议时为零。
 */
int main(void)
{
    char message[256];
    ChaliAbiError error = { message, sizeof(message), 0, 0 };
    ChaliAbiRegistry *registry = chali_abi_registry_create(&error);
    ChaliAbiInfo abi = chali_abi_local_info(0);
    char owned_name[] = "host::add";
    const ChaliAbiRecord records[] = { { owned_name, add, 2 }, { "host::fail", fail, 0 }, { "host::invalid", invalid_result, 0 } };
    const ChaliAbiModuleDescriptor module = { sizeof(module), &abi, records, 3 };
    int64_t values[2] = { 20, 22 };
    unsigned char kinds[2] = { ChaliAbiSlotInteger, ChaliAbiSlotInteger };
    ChaliAbiFrame frame = { NULL, values, kinds, 2, 0, 0 };
    ChaliAbiEntry resolved = NULL;
    int32_t arity = 0;
    int failed = 0;
#define REQUIRE(condition)                                             \
    do                                                                 \
    {                                                                  \
        if (!(condition))                                              \
        {                                                              \
            fprintf(stderr, "C API line %d: %s\n", __LINE__, message); \
            failed = 1;                                                \
            goto finish;                                               \
        }                                                              \
    } while (0)
    REQUIRE(registry != NULL);
    REQUIRE(chali_abi_registry_register(registry, &module, &error) == ChaliAbiOk);
    owned_name[0] = 'X'; /* 注册表持有名称副本。 */
    REQUIRE(chali_abi_registry_resolve(registry, "host::add", &resolved, &arity, &error) == ChaliAbiOk);
    REQUIRE(resolved == add && arity == 2);
    REQUIRE(chali_abi_registry_call(registry, "host::add", &frame, &error) == ChaliAbiOk);
    REQUIRE(frame.result == 42 && frame.runtime == NULL && entered == 1);
    frame.count = 1;
    REQUIRE(chali_abi_registry_call(registry, "host::add", &frame, &error) == ChaliAbiInvalidArgument);
    REQUIRE(entered == 1 && frame.result == 0);
    frame.count = 2;
    kinds[1] = ChaliAbiSlotFloat;
    REQUIRE(chali_abi_registry_call(registry, "host::add", &frame, &error) == ChaliAbiHostFailure);
    REQUIRE(entered == 1 && frame.result == 0);
    kinds[1] = ChaliAbiSlotInteger;
    {
        const ChaliAbiRecord conflicting[] = { { "host::temporary", add, 2 }, { "host::add", add, 2 } };
        const ChaliAbiModuleDescriptor rejected = { sizeof(rejected), &abi, conflicting, 2 };
        REQUIRE(chali_abi_registry_register(registry, &rejected, &error) == ChaliAbiDuplicate);
        REQUIRE(chali_abi_registry_resolve(registry, "host::temporary", &resolved, &arity, &error) == ChaliAbiNotFound);
        REQUIRE(resolved == NULL && arity == 0);
    }
    frame.count = 0;
    REQUIRE(chali_abi_registry_call(registry, "host::fail", &frame, &error) == ChaliAbiHostFailure);
    REQUIRE(strcmp(message, "C host requested failure") == 0 && frame.result == 0 && frame.runtime == NULL);
    REQUIRE(chali_abi_registry_call(registry, "host::invalid", &frame, &error) == ChaliAbiInvalidArgument);
    REQUIRE(frame.result == 0 && frame.resultIsFloat == 0);
    REQUIRE(chali_abi_registry_seal(registry, &error) == ChaliAbiOk);
    REQUIRE(chali_abi_registry_seal(registry, &error) == ChaliAbiOk);
    REQUIRE(chali_abi_registry_register(registry, &module, &error) == ChaliAbiSealed);
    frame.count = 2;
    REQUIRE(chali_abi_registry_call(registry, "host::add", &frame, &error) == ChaliAbiOk && frame.result == 42);
    puts("opaque C registry: 42; atomic duplicate rejection and failure reset passed");
finish:
    chali_abi_registry_destroy(registry);
    return failed;
#undef REQUIRE
}
