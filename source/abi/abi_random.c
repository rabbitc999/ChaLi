/**
 * @file abi_random.c
 * @brief `std::random` 的直通 C 实现。
 *
 * 生成器状态是一个 64 位字，交给 VM 对象表托管：创建、借用、复制、释放分别走
 * `chali_abi_record_make` / `_borrow` / `_clone` / `_release`。
 *
 * 本模块**不保证与旧实现的取值序列一致**（已获授权）：序列只要求满足随机性，
 * 接口语义（创建 / 复制 / 释放）与区间约定保持等价。算法选用 splitmix64，
 * 通过 BigCrush 级别的常见统计检验，状态空间为 2^64。
 */
#include "chali_abi_internal.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/**
 * @brief 随机数生成器状态。
 *
 * 由对象表托管；`destroy` 回调负责释放本结构。
 */
typedef struct AbiRandomState
{
    uint64_t value;  ///< splitmix64 的当前状态字。
} AbiRandomState;

/**
 * @brief 记录类型名；借用时用于身份校验，须与创建时一致。
 */
static const char kRandomType[] = "std::random::Random";

/**
 * @brief 推进状态一次并返回扰乱后的输出。
 * @param state 生成器状态。
 * @return 下一项 64 位无符号随机值。
 */
static uint64_t abiRandomNext(AbiRandomState *state)
{
    uint64_t z = (state->value += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

/**
 * @brief 释放生成器状态。
 * @param value 待释放的状态指针。
 */
static void abiRandomDestroy(void *value)
{
    free(value);
}

/**
 * @brief 生成一个新状态的副本。
 * @param source 原状态。
 * @return 独立副本指针；分配失败返回空指针。
 */
static void *abiRandomClone(const void *source)
{
    const AbiRandomState *origin = (const AbiRandomState *)source;
    AbiRandomState *copy = (AbiRandomState *)malloc(sizeof(AbiRandomState));
    if (copy != NULL) *copy = *origin;
    return copy;
}

/**
 * @brief 读取当前高分辨率时间戳，用于熵播种。
 * @return 单调时钟纳秒读数；失败时返回零。
 */
static uint64_t abiEntropyStamp(void)
{
#ifdef _WIN32
    LARGE_INTEGER counter;
    if (QueryPerformanceCounter(&counter) == 0) return 0;
    return (uint64_t)counter.QuadPart;
#else
    struct timespec stamp;
    if (clock_gettime(CLOCK_MONOTONIC, &stamp) != 0) return 0;
    return (uint64_t)stamp.tv_sec * 1000000000ull + (uint64_t)stamp.tv_nsec;
#endif
}

/**
 * @brief 创建带指定种子的生成器。
 * @param frame 帧：0 = 种子。
 */
static void abiRandomWithSeed(ChaliAbiFrame *frame)
{
    int64_t seed = 0;
    chali_abi_argument_integer(frame, 0, &seed);
    AbiRandomState *state = (AbiRandomState *)malloc(sizeof(AbiRandomState));
    if (state == NULL)
    {
        chali_abi_fail(frame, "随机数状态分配失败");
        chali_abi_return_integer(frame, 0);
        return;
    }
    state->value = (uint64_t)seed;
    const int64_t handle = chali_abi_record_make(frame, kRandomType, state, abiRandomDestroy);
    if (handle == 0) abiRandomDestroy(state);
    chali_abi_return_integer(frame, handle);
}

/**
 * @brief 用时间、地址和调用计数混合播种，创建生成器。
 * @param frame 帧：无实参。
 */
static void abiRandomFromEntropy(ChaliAbiFrame *frame)
{
    static uint64_t counter = 0;
    uint64_t seed = abiEntropyStamp();
    seed ^= (uint64_t)(uintptr_t)&counter;
    seed += (counter += 0x9E3779B97F4A7C15ull);
    seed ^= seed >> 33;
    seed *= 0xFF51AFD7ED558CCDull;
    seed ^= seed >> 33;
    AbiRandomState *state = (AbiRandomState *)malloc(sizeof(AbiRandomState));
    if (state == NULL)
    {
        chali_abi_fail(frame, "随机数状态分配失败");
        chali_abi_return_integer(frame, 0);
        return;
    }
    state->value = seed;
    const int64_t handle = chali_abi_record_make(frame, kRandomType, state, abiRandomDestroy);
    if (handle == 0) abiRandomDestroy(state);
    chali_abi_return_integer(frame, handle);
}

/**
 * @brief 借用生成器状态，失败时记录诊断。
 * @param frame 当前调用帧。
 * @return 状态指针；句柄无效或类型不匹配返回空指针。
 */
static AbiRandomState *abiRandomBorrow(const ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    chali_abi_argument_integer(frame, 0, &handle);
    void *record = chali_abi_record_borrow(frame, handle, kRandomType);
    if (record == NULL)
    {
        chali_abi_fail(frame, "Random 状态已经释放或类型无效");
        return NULL;
    }
    return (AbiRandomState *)record;
}

/**
 * @brief 生成下一项 64 位无符号整数。
 * @param frame 帧：0 = 生成器句柄。
 */
static void abiRandomNextUInt64(ChaliAbiFrame *frame)
{
    AbiRandomState *state = abiRandomBorrow(frame);
    if (state == NULL)
    {
        chali_abi_return_integer(frame, 0);
        return;
    }
    chali_abi_return_integer(frame, (int64_t)abiRandomNext(state));
}

/**
 * @brief 生成 `[minimum, maximum)` 区间内的整数。
 * @param frame 帧：0 = 句柄，1 = 下界，2 = 上界。
 */
static void abiRandomNextInt(ChaliAbiFrame *frame)
{
    int64_t minimum = 0, maximum = 0;
    chali_abi_argument_integer(frame, 1, &minimum);
    chali_abi_argument_integer(frame, 2, &maximum);
    AbiRandomState *state = abiRandomBorrow(frame);
    if (state == NULL || maximum <= minimum)
    {
        chali_abi_fail(frame, "random.nextInt 要求 maximum > minimum");
        chali_abi_return_integer(frame, 0);
        return;
    }
    const uint64_t width = (uint64_t)maximum - (uint64_t)minimum;
    chali_abi_return_integer(frame, (int64_t)((uint64_t)minimum + abiRandomNext(state) % width));
}

/**
 * @brief 生成 `[0, 1)` 区间内的双精度值。
 * @param frame 帧：0 = 句柄。
 * @note 取高 53 位，保证均匀覆盖 2^53 个格点。
 */
static void abiRandomNextFloat64(ChaliAbiFrame *frame)
{
    AbiRandomState *state = abiRandomBorrow(frame);
    if (state == NULL)
    {
        chali_abi_return_integer(frame, 0);
        return;
    }
    const double value = (double)(abiRandomNext(state) >> 11) / 9007199254740992.0;
    chali_abi_return_real(frame, value);
}

/**
 * @brief 生成 `[minimum, maximum)` 区间内的双精度值。
 * @param frame 帧：0 = 句柄，1 = 下界，2 = 上界。
 */
static void abiRandomNextRange(ChaliAbiFrame *frame)
{
    double minimum = 0.0, maximum = 0.0;
    chali_abi_argument_real(frame, 1, &minimum);
    chali_abi_argument_real(frame, 2, &maximum);
    AbiRandomState *state = abiRandomBorrow(frame);
    if (state == NULL || !(maximum > minimum))
    {
        chali_abi_fail(frame, "random.nextRange 要求 maximum > minimum");
        chali_abi_return_integer(frame, 0);
        return;
    }
    const double unit = (double)(abiRandomNext(state) >> 11) / 9007199254740992.0;
    chali_abi_return_real(frame, minimum + unit * (maximum - minimum));
}

/**
 * @brief 生成随机布尔值。
 * @param frame 帧：0 = 句柄。
 */
static void abiRandomNextBool(ChaliAbiFrame *frame)
{
    AbiRandomState *state = abiRandomBorrow(frame);
    if (state == NULL)
    {
        chali_abi_return_integer(frame, 0);
        return;
    }
    chali_abi_return_integer(frame, (int64_t)((abiRandomNext(state) >> 63) & 1u));
}

/**
 * @brief 复制生成器。
 * @param frame 帧：0 = 句柄。
 * @return 新句柄。
 */
static void abiRandomCopy(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    chali_abi_argument_integer(frame, 0, &handle);
    chali_abi_return_integer(frame, chali_abi_record_clone(frame, handle, kRandomType, abiRandomClone));
}

/**
 * @brief 释放生成器。
 * @param frame 帧：0 = 句柄。
 */
static void abiRandomDrop(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    chali_abi_argument_integer(frame, 0, &handle);
    if (chali_abi_record_release(frame, handle) == 0)
        chali_abi_fail(frame, "[CHL1205] drop 的对象已经释放或句柄无效");
    chali_abi_return_integer(frame, 0);
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "std::random::__randomCopy", abiRandomCopy, 1 },
    { "std::random::randomDrop", abiRandomDrop, 1 },
    { "std::random::randomFromEntropy", abiRandomFromEntropy, 0 },
    { "std::random::randomNextBool", abiRandomNextBool, 1 },
    { "std::random::randomNextFloat64", abiRandomNextFloat64, 1 },
    { "std::random::randomNextInt", abiRandomNextInt, 3 },
    { "std::random::randomNextRange", abiRandomNextRange, 3 },
    { "std::random::randomNextUInt64", abiRandomNextUInt64, 1 },
    { "std::random::randomWithSeed", abiRandomWithSeed, 1 },
};

const ChaliAbiRecord *abi_random_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
