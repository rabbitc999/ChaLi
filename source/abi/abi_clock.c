/**
 * @file abi_clock.c
 * @brief `std::clock` 的直通 C 实现。
 *
 * 读数取自单调时钟，不受系统挂钟调整影响；与旧实现的取值基准一致
 * （均为"自实现基准起的纳秒数"，只用于差值计算）。
 */
#include "chali_abi_internal.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/**
 * @brief 读取当前单调时钟的纳秒读数。
 * @return 自该时钟实现基准起经过的纳秒数；读取失败返回零。
 */
static int64_t monotonicNanoseconds(void)
{
#ifdef _WIN32
    LARGE_INTEGER counter, frequency;
    if (QueryPerformanceFrequency(&frequency) == 0 || frequency.QuadPart <= 0) return 0;
    if (QueryPerformanceCounter(&counter) == 0) return 0;
    const int64_t whole = (int64_t)(counter.QuadPart / frequency.QuadPart);
    const int64_t remainder = (int64_t)(counter.QuadPart % frequency.QuadPart);
    return whole * 1000000000ll + remainder * 1000000000ll / frequency.QuadPart;
#else
    struct timespec stamp;
    if (clock_gettime(CLOCK_MONOTONIC, &stamp) != 0) return 0;
    return (int64_t)stamp.tv_sec * 1000000000ll + (int64_t)stamp.tv_nsec;
#endif
}

/**
 * @brief 单调时钟纳秒读数。
 * @param frame 帧：无实参。
 */
static void abiClockNanos(ChaliAbiFrame *frame)
{
    chali_abi_return_integer(frame, monotonicNanoseconds());
}

/**
 * @brief 单调时钟毫秒读数。
 * @param frame 帧：无实参。
 */
static void abiClockMillis(ChaliAbiFrame *frame)
{
    chali_abi_return_integer(frame, monotonicNanoseconds() / 1000000ll);
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "std::clock::nowMs", abiClockMillis, 0 },
    { "std::clock::nowNanos", abiClockNanos, 0 },
};

const ChaliAbiRecord *abi_clock_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
