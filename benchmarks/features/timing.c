/** @file timing.c
 * @brief ChaLi 与 C++ 共用的纯访问计时壳，构造和校验归并均不计时。
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
void feature_setup(void);
int64_t feature_work(int64_t seed);
#ifdef __cplusplus
}
#endif

/**
 * @brief 读取单调高精度时钟。
 * @return 单调纳秒数。
 */
static int64_t now_ns(void)
{
#ifdef _WIN32
    LARGE_INTEGER frequency, ticks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&ticks);
    return ticks.QuadPart / frequency.QuadPart * 1000000000LL
           + ticks.QuadPart % frequency.QuadPart * 1000000000LL / frequency.QuadPart;
#else
    struct timespec value;
    clock_gettime(CLOCK_MONOTONIC, &value);
    return (int64_t)value.tv_sec * 1000000000LL + value.tv_nsec;
#endif
}

#ifdef CHALI_FEATURE_AOT
void chali_init(void);
void chali_finish(void);
#endif
/**
 * @brief 构造数据、预热一次，再单独测量五次工作函数。
 * @param argc 命令行参数数量。
 * @param argv 传入 --verify 时仅核对逐个种子结果，不计时。
 * @return 正常完成时返回零。
 */
int main(int argc, char **argv)
{
#ifdef CHALI_FEATURE_AOT
    chali_init();
#endif
    feature_setup();
    int64_t (*volatile kernel)(int64_t) = feature_work;
    int64_t sink = kernel(0);
    if (argc > 1 && strcmp(argv[1], "--verify") == 0)
    {
        for (int64_t seed = 0; seed < 5; ++seed)
            printf("value %lld %lld\n", (long long)seed, (long long)kernel(seed));
#ifdef CHALI_FEATURE_AOT
        chali_finish();
#endif
        return 0;
    }
    for (int64_t seed = 0; seed < 5; ++seed)
    {
        const int64_t start = now_ns();
        const int64_t value = kernel(seed);
        const int64_t elapsed = now_ns() - start;
        sink += value;
        printf("rep %lld\n", (long long)elapsed);
    }
    printf("sink %lld\n", (long long)sink);
#ifdef CHALI_FEATURE_AOT
    chali_finish();
#endif
    return 0;
}
