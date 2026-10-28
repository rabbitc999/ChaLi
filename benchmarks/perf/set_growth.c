/* set_growth：C 开放寻址参考，使用系统 calloc/free。 */
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "hash_int.h"
#define N 20000
#define REPS 5
/**
 * @brief 生成分散且唯一的正整数键。
 * @param index 操作顺序。
 * @param seed 本轮初值。
 * @return 十亿附近的整数键。
 */
static int64_t key(int64_t index,int64_t seed)
{ return (index*65537+seed*17)%1000000007+1000000007; }
/**
 * @brief 从空表执行增长、重复插入、查询、删除和重新插入。
 * @param seed 本轮初值。
 * @return 操作结果的有序校验值。
 */
static int64_t work(int64_t seed)
{
    PerfHashTable values={0};
    int64_t total=seed;
    for (int64_t i=0;i<N;++i) perf_put(&values,key(i,seed),1);
    for (int64_t i=0;i<N;++i) perf_put(&values,key(i,seed),1);
    for (int64_t i=0;i<N;++i)
    {
        int64_t k=key(i,seed);
        total=(total*31+(perf_get(&values,k)!=NULL)+2*(perf_get(&values,-k)!=NULL))&2147483647;
    }
    for (int64_t i=0;i<N;i+=3) total+=perf_remove(&values,key(i,seed));
    for (int64_t i=0;i<N;i+=3) perf_put(&values,key(i,seed),1);
    for (int64_t i=0;i<N;++i) total=(total*31+(perf_get(&values,key(i,seed))!=NULL))&2147483647;
    free(values.entries);
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
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
/**
 * @brief 预热后通过易变函数入口计时，归并跨语言校验值。
 * @return 完成五次测量后返回零。
 */
int main(void)
{
    int64_t (*volatile kernel)(int64_t) = work;
    volatile int64_t sink = kernel(0);
    for (int r = 0; r < REPS; ++r)
    {
        long long start = now_ns();
        sink += kernel(r);
        long long elapsed = now_ns() - start;
        printf("rep %lld\n", elapsed);
    }
    printf("sink %lld\n", (long long)sink);
    return 0;
}
