/**
 * @file chali_thread_c_test.c
 * @brief 验证 C 线程接口的入口、等待可见性与失败参数处理。
 */
#include "../source/abi/chali_thread.h"
#include "../source/abi/chali_thread_service.h"
#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdio.h>

/**
 * @brief 在线程内更新由调用方持有的测试值。
 * @param argument 整数地址，生命周期覆盖线程等待。
 */
static void set_value(void *argument)
{
    *(int *)argument = 42;
}
/** @brief 自等待检查使用的发布状态。 */
struct SelfWait
{
    atomic_int ready;
    ChaliThread *thread;
    int result;
};
/**
 * @brief 在接收到自身句柄后验证自等待被拒绝。
 * @param argument 调用方持有的发布状态。
 */
static void join_self(void *argument)
{
    struct SelfWait *state = (struct SelfWait *)argument;
    while (!atomic_load_explicit(&state->ready, memory_order_acquire)) {}
    state->result = chali_thread_join(state->thread);
}
/**
 * @brief 重复创建并回收线程，核对所有权转移和等待后的内存可见性。
 * @return 全部通过时返回零。
 */
int main(void)
{
    ChaliThread *thread = NULL;
    assert(chali_thread_create(NULL, NULL, &thread) != 0 && thread == NULL);
    assert(chali_thread_create(set_value, NULL, NULL) != 0);
    assert(chali_thread_join(NULL) != 0);
    for (int index = 0; index < 100; ++index)
    {
        int value = 0;
        assert(chali_thread_create(set_value, &value, &thread) == 0);
        assert(thread != NULL);
        assert(chali_thread_join(thread) == 0);
        thread = NULL;
        assert(value == 42);
    }
    struct SelfWait state;
    atomic_init(&state.ready, 0);
    state.thread = NULL;
    state.result = 0;
    assert(chali_thread_create(join_self, &state, &state.thread) == 0);
    atomic_store_explicit(&state.ready, 1, memory_order_release);
    assert(chali_thread_join(state.thread) == 0);
    assert(state.result == EDEADLK);
    puts("C thread service: 100 create/join cycles, self-wait and invalid arguments passed");
    return 0;
}
