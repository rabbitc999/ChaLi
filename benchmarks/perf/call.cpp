/* call —— 小函数调用，1000000 轮。C++17，g++ -O2。 */
#include <chrono>
#include <cstdint>
#include <cstdio>

namespace
{
constexpr std::int64_t N = 1000000;
constexpr int REPS = 5;

/**
 * @brief 执行小函数调用负载中的乘加计算。
 * @param x 待混合的整数。
 * @return 输入乘以十七后加三的结果。
 */
std::int64_t mix(int64_t x) { return x * 17 + 3; }

/**
 * @brief 循环调用整数乘加函数并累加掩码结果，生成跨语言校验值。
 * @param seed 各次测量的不同初值，防止跨轮常量复用。
 * @return 计算结果，供计时入口归并校验。
 */
std::int64_t work(int64_t seed)
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
std::int64_t now_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}
}  // namespace

/**
 * @brief 通过易变函数入口预热并计时，阻止计算越过计时边界。
 * @return 成功完成五次采样后返回零。
 */
int main()
{
    int64_t (*volatile kernel)(int64_t) = work;
    volatile int64_t sink = kernel(0);
    for (int r = 0; r < REPS; ++r)
    {
        const std::int64_t t0 = now_ns();
        sink += kernel(r);
        const std::int64_t t1 = now_ns();
        std::printf("rep %lld\n", static_cast<long long>(t1 - t0));
    }
    std::printf("sink %lld\n", (long long)sink);
    return 0;
}
