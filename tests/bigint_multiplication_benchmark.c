/** @file bigint_multiplication_benchmark.c @brief 进程内批量计时与工作内存统计的 C 乘法基准。 */
#ifndef _WIN32
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#define CHALI_BIGINT_TESTING 1
#include "chali_bigint.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sched.h>
#include <time.h>
#endif
/** @brief 单线程基准的分配统计，不参与产品运行时。 */
typedef struct AllocationStats
{
    size_t live, peak, calls;
} AllocationStats;
/**
 * @brief 分配并记录活动量及峰值。
 * @param context 分配统计。
 * @param bytes 请求字节数。
 * @return C 堆地址或空指针。
 */
static void *benchmark_allocate(void *context, size_t bytes)
{
    AllocationStats *stats = context;
    void *pointer = malloc(bytes);
    if (pointer)
    {
        stats->live += bytes;
        if (stats->live > stats->peak) stats->peak = stats->live;
        ++stats->calls;
    }
    return pointer;
}
/**
 * @brief 按原容量释放 C 存储。
 * @param context 分配统计。
 * @param pointer 独占地址。
 * @param bytes 原分配大小。
 */
static void benchmark_release(void *context, void *pointer, size_t bytes)
{
    AllocationStats *stats = context;
    if (pointer)
    {
        if (bytes > stats->live) abort();
        stats->live -= bytes;
        free(pointer);
    }
}
#ifdef _WIN32
static double nanoseconds_per_tick;
#endif
/**
 * @brief 读取单调高分辨率计时器。
 * @return 纳秒计数。
 */
static uint64_t benchmark_now(void)
{
#ifdef _WIN32
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return (uint64_t)(value.QuadPart * nanoseconds_per_tick);
#else
    struct timespec value;
    clock_gettime(CLOCK_MONOTONIC, &value);
    return (uint64_t)value.tv_sec * UINT64_C(1000000000) + (uint64_t)value.tv_nsec;
#endif
}
/**
 * @brief 读取本线程实际消耗的时钟，排除未获调度的时间。
 * @return Windows 为线程周期数，Linux 为线程 CPU 纳秒。
 */
static uint64_t benchmark_cpu_time(void)
{
#ifdef _WIN32
    ULONG64 cycles = 0;
    QueryThreadCycleTime(GetCurrentThread(), &cycles);
    return (uint64_t)cycles;
#else
    struct timespec value;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &value);
    return (uint64_t)value.tv_sec * UINT64_C(1000000000) + (uint64_t)value.tv_nsec;
#endif
}
/**
 * @brief 固定到可用的一个逻辑处理器，减少迁核噪声，不改变系统设置。
 */
static void benchmark_pin(void)
{
    const char *requested = getenv("CHALI_BENCH_CPU");
    const int preferred = requested ? atoi(requested) : 1;
#ifdef _WIN32
    DWORD_PTR allowed, system, selected;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    nanoseconds_per_tick = 1e9 / (double)frequency.QuadPart;
    if (GetProcessAffinityMask(GetCurrentProcess(), &allowed, &system))
    {
        selected = preferred >= 0 && preferred < (int)(sizeof(DWORD_PTR) * 8) ? (DWORD_PTR)1 << preferred : 0;
        selected = (allowed & selected) ? selected : (allowed & (~allowed + 1));
        SetThreadAffinityMask(GetCurrentThread(), selected);
    }
#else
    cpu_set_t allowed, selected;
    int i;
    if (sched_getaffinity(0, sizeof(allowed), &allowed) == 0)
    {
        int chosen = -1;
        if (preferred >= 0 && preferred < CPU_SETSIZE && CPU_ISSET(preferred, &allowed))
            chosen = preferred;
        else
            for (i = 0; i < CPU_SETSIZE; ++i)
                if (CPU_ISSET(i, &allowed))
                {
                    chosen = i;
                    break;
                }
        if (chosen >= 0)
        {
            CPU_ZERO(&selected);
            CPU_SET(chosen, &selected);
            sched_setaffinity(0, sizeof(selected), &selected);
        }
    }
#endif
}
/**
 * @brief 生成固定种子的伪随机分块。
 * @param state 非零随机状态。
 * @return 下一个三十二位分块。
 */
static uint32_t benchmark_random(uint64_t *state)
{
    *state ^= *state << 13;
    *state ^= *state >> 7;
    *state ^= *state << 17;
    return (uint32_t)*state;
}
/**
 * @brief 在计时之外直接构造指定分块长度的正数。
 * @param value 已初始化目标。
 * @param words 分块长度。
 * @param state 随机状态。
 * @param pattern 零随机、一全一、二交替位、三稀疏。
 * @return 操作状态。
 */
static ChaliCStatus benchmark_value(ChaliBigInt *value, size_t words, uint64_t *state, int pattern)
{
    uint32_t *data;
    size_t i;
    ChaliCStatus status = chali_c_buffer_resize(&value->limbs, words);
    if (status != CHALI_C_OK) return status;
    data = value->limbs.data;
    for (i = 0; i < words; ++i)
        data[i] = pattern == 1 ? UINT32_MAX : pattern == 2 ? UINT32_C(0xaaaaaaaa) :
                                          pattern == 3     ? (i % 32 == 0 ? 1u : 0u) :
                                                             benchmark_random(state);
    data[words - 1] |= UINT32_C(0x80000000);
    value->sign = 1;
    return CHALI_C_OK;
}
/**
 * @brief 调用当前或冻结基线的乘法实现。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @param route 顶层算法编号。
 * @return 操作状态。
 */
static ChaliCStatus benchmark_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int route)
{
#ifdef CHALI_BIGINT_BENCHMARK_BASELINE
    ChaliCStatus status;
    const size_t previous = chali_bigint_multiplication_threshold();
    if (route == 2)
        chali_bigint_set_multiplication_threshold(SIZE_MAX);
    else if (route == 4)
        chali_bigint_set_multiplication_threshold(1);
    status = chali_bigint_multiply(output, left, right);
    chali_bigint_set_multiplication_threshold(previous);
    return status;
#else
    if (!route) return chali_bigint_multiply(output, left, right);
    return chali_bigint_test_multiply(output, left, right, route);
#endif
}
/**
 * @brief 检查状态并在失败时保留可定位的输入规模。
 * @param status C 状态。
 * @param a 左分块数。
 * @param b 右分块数。
 * @param route 算法编号。
 */
static void benchmark_check(ChaliCStatus status, size_t a, size_t b, int route)
{
    if (status != CHALI_C_OK)
    {
        fprintf(stderr, "multiply failed: %zu %zu route=%d status=%d\n", a, b, route, status);
        exit(2);
    }
}
/**
 * @brief 测量一组输入，在每个样本结束后核对完整乘积。
 * @param a_words 左分块数。
 * @param b_words 右分块数。
 * @param square 是否平方。
 * @param route 顶层算法编号。
 * @param target_us 每批目标微秒数。
 * @param samples 样本数。
 * @param seed 固定随机种子。
 * @param pattern 数据模式。
 */
static void benchmark_case(size_t a_words, size_t b_words, int square, int route, size_t target_us,
                           int samples, uint64_t seed, int pattern)
{
    AllocationStats stats = { 0 };
    ChaliCAllocator allocator = { &stats, benchmark_allocate, benchmark_release };
    ChaliBigInt a, b, result, reference;
    const ChaliBigInt *right;
    size_t i, iterations, base_live;
    uint64_t begin, elapsed, cpu_begin, cpu_elapsed;
    int sample;
    chali_bigint_init(&a, &allocator);
    chali_bigint_init(&b, &allocator);
    chali_bigint_init(&result, &allocator);
    chali_bigint_init(&reference, &allocator);
    benchmark_check(benchmark_value(&a, a_words, &seed, pattern), a_words, b_words, route);
    if (!square) benchmark_check(benchmark_value(&b, b_words, &seed, pattern), a_words, b_words, route);
    right = square ? &a : &b;
    if (!square && chali_bigint_compare(&a, &b) == 0) ((uint32_t *)b.limbs.data)[0] ^= 1;
#ifndef CHALI_BIGINT_BENCHMARK_BASELINE
    if (!route) printf("# route,%zu,%zu,%d,%d,%d\n", a_words, b_words, square, pattern, chali_bigint_test_multiplication_route(&a, right));
#endif
    benchmark_check(benchmark_multiply(&reference, &a, right, 2), a_words, b_words, 2);
    begin = benchmark_now();
    for (i = 0; i < 4; ++i) benchmark_check(benchmark_multiply(&result, &a, right, route), a_words, b_words, route);
    elapsed = benchmark_now() - begin;
    iterations = elapsed ? (size_t)((uint64_t)target_us * 1000 * 4 / elapsed) : 200000;
    if (!iterations) iterations = 1;
    if (iterations > 200000) iterations = 200000;
    base_live = stats.live;
    for (sample = 0; sample < samples; ++sample)
    {
        stats.peak = stats.live;
        stats.calls = 0;
        cpu_begin = benchmark_cpu_time();
        begin = benchmark_now();
        for (i = 0; i < iterations; ++i)
            benchmark_check(benchmark_multiply(&result, &a, right, route), a_words, b_words, route);
        elapsed = benchmark_now() - begin;
        cpu_elapsed = benchmark_cpu_time() - cpu_begin;
        if (chali_bigint_compare(&result, &reference) != 0)
        {
            fprintf(stderr, "product mismatch\n");
            exit(3);
        }
        printf("%zu,%zu,%d,%d,%d,%d,%zu,%.3f,%.3f,%zu,%.3f\n", a_words, b_words, square, route, pattern, sample,
               iterations, (double)elapsed / iterations, (double)stats.calls / iterations, stats.peak - base_live,
               (double)cpu_elapsed / iterations);
    }
    chali_bigint_clear(&a);
    chali_bigint_clear(&b);
    chali_bigint_clear(&result);
    chali_bigint_clear(&reference);
    if (stats.live)
    {
        fprintf(stderr, "leaked bytes: %zu\n", stats.live);
        exit(4);
    }
    fflush(stdout);
}
/**
 * @brief 读取策略和基准请求；每行输出实际计时和工作内存统计。
 * @return 所有乘积和释放均正确时返回零。
 */
int main(void)
{
    char command[16];
    benchmark_pin();
#ifndef CHALI_BIGINT_BENCHMARK_BASELINE
    printf("# defaults,%zu,%zu,%zu,%zu,%zu,%zu\n", chali_bigint_schoolbook_threshold(), chali_bigint_square_threshold(),
           chali_bigint_toom_threshold(), chali_bigint_toom_square_threshold(), chali_bigint_multiplication_threshold(),
           chali_bigint_karatsuba_threshold());
#endif
    puts("# a_words,b_words,square,route,pattern,sample,iterations,ns,allocations,peak_extra_bytes,cpu_ticks");
    while (scanf("%15s", command) == 1)
    {
        if (!strcmp(command, "policy"))
        {
            size_t school, square, toom, toom_square, ntt, guard;
            if (scanf("%zu%zu%zu%zu%zu%zu", &school, &square, &toom, &toom_square, &ntt, &guard) != 6) return 5;
#ifndef CHALI_BIGINT_BENCHMARK_BASELINE
            chali_bigint_set_schoolbook_threshold(school);
            chali_bigint_set_square_threshold(square);
            chali_bigint_set_toom_threshold(toom);
            chali_bigint_set_toom_square_threshold(toom_square);
#else
            (void)school;
            (void)square;
            (void)toom;
            (void)toom_square;
#endif
            chali_bigint_set_multiplication_threshold(ntt);
            chali_bigint_set_karatsuba_threshold(guard);
        }
        else if (!strcmp(command, "bench"))
        {
            size_t a, b, target_us;
            int square, route, samples, pattern;
            uint64_t seed;
            if (scanf("%zu%zu%d%d%zu%d%" SCNu64 "%d", &a, &b, &square, &route, &target_us, &samples, &seed, &pattern) != 8)
                return 5;
            if (!a || !b || a > 65536 || b > 65536 || route < 0 || route > 4 || samples < 1 || samples > 31 || target_us < 1 || target_us > 1000000 || pattern < 0 || pattern > 3 || (square && a != b)) return 5;
            if (!seed) seed = 1;
            benchmark_case(a, b, square, route, target_us, samples, seed, pattern);
        }
        else
            return 5;
    }
    return 0;
}
