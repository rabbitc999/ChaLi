/* call —— 小函数调用，1000000 轮。C11，gcc -O2。 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N    1000000
#define REPS 5

/**
 * @brief 执行小函数调用负载中的乘加计算。
 * @param x 待混合的整数。
 * @return 输入乘以十七后加三的结果。
 */
static int64_t mix(int64_t x) { return x * 17 + 3; }

/**
 * @brief 循环调用整数乘加函数并累加掩码结果，生成跨语言校验值。
 * @param seed 各次测量的不同初值，防止跨轮常量复用。
 * @return 计算结果，供计时入口归并校验。
 */
static int64_t work(int64_t seed)
{
    int64_t acc = seed;
    for (int64_t i = 0; i < N; i++)
    {
        acc = (mix(acc) + i) & 2147483647;
    }
    return acc;
}

/**
 * @brief 读取宿主单调时钟。
 * @return 当前单调时间的纳秒读数。
 */
static long long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
}

/**
 * @brief 通过易变函数入口预热并计时，阻止计算越过计时边界。
 * @return 成功完成五次采样后返回零。
 */
int main(void)
{
    int64_t (*volatile kernel)(int64_t) = work;
    volatile int64_t sink = kernel(0);
    for (int r = 0; r < REPS; r++)
    {
        long long t0 = now_ns();
        sink += kernel(r);
        long long t1 = now_ns();
        printf("rep %lld\n", t1 - t0);
    }
    printf("sink %lld\n", (long long)sink);
    return 0;
}
