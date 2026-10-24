/**
 * @file array_rw.c
 * @brief 与 ChaLi 相同的固定数组数据相关读写性能负载。
 */
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define N    4000000
#define REPS 5

/**
 * @brief 初始化数组后执行固定轮数的读写递推。
 * @param seed 同一进程各次测量使用的不同初值。
 * @return 用于跨语言校验的最终累加值。
 */
static int64_t work(int64_t seed)
{
    int64_t values[16];
    for (int64_t k = 0; k < 16; ++k) values[k] = k * 3;
    int64_t acc = seed;
    for (int64_t i = 0; i < N; ++i)
    {
        const int64_t index = acc & 15;
        values[index] = (values[index] + i) & 2147483647;
        acc = (acc + values[index]) & 2147483647;
    }
    return acc;
}

/**
 * @brief 读取宿主单调时钟。
 * @return 当前单调时间的纳秒读数。
 */
static long long now_ns(void)
{
    struct timespec ticks;
    clock_gettime(CLOCK_MONOTONIC, &ticks);
    return (long long)ticks.tv_sec * 1000000000LL + ticks.tv_nsec;
}

/**
 * @brief 预热后采集五次计算时长并输出校验值。
 * @return 成功返回零。
 */
int main(void)
{
    int64_t (*volatile kernel)(int64_t) = work;
    volatile int64_t sink = kernel(0);
    for (int r = 0; r < REPS; ++r)
    {
        const long long start = now_ns();
        sink += kernel(r);
        const long long elapsed = now_ns() - start;
        printf("rep %lld\n", elapsed);
    }
    printf("sink %lld\n", (long long)sink);
    return 0;
}
