/* list_growth —— 动态数组 push 再遍历求和，400000 轮。C11，gcc -O2。 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N    400000
#define REPS 5

/**
 * @brief 动态追加数组、随后按顺序累乘归并，生成跨语言校验值。
 * @param seed 各次测量的不同初值，防止跨轮常量复用。
 * @return 计算结果，供计时入口归并校验。
 */
static int64_t work(int64_t seed)
{
    int64_t *xs = NULL;
    size_t len = 0, capacity = 0;
    for (int64_t i = 0; i < N; i++)
    {
        if (len == capacity)
        {
            capacity = capacity ? capacity * 2 : 4;
            int64_t *grown = realloc(xs, sizeof(int64_t) * capacity);
            if (!grown) abort();
            xs = grown;
        }
        xs[len++] = i * 3 + seed;
    }
    int64_t total = seed;
    for (size_t i = 0; i < len; i++)
    {
        total = (total * 31 + xs[i]) & 2147483647;
    }
    free(xs);
    return total;
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
