/* list —— 动态数组 push 再遍历求和，400000 轮。C++17，g++ -O2。 */
#include <chrono>
#include <cstdint>
#include <cstdio>

namespace
{
constexpr std::int64_t N = 400000;
constexpr int REPS = 5;

/**
 * @brief 预分配数组、顺序写入后按顺序累乘归并，生成跨语言校验值。
 * @param seed 各次测量的不同初值，防止跨轮常量复用。
 * @return 计算结果，供计时入口归并校验。
 */
std::int64_t work(int64_t seed)
{
    std::int64_t *xs = new std::int64_t[N];
    std::size_t len = 0;
    for (int64_t i = 0; i < N; i++) { xs[len++] = i * 3 + seed; }
    int64_t total = seed;
    for (std::size_t i = 0; i < len; i++)
    {
        total = (total * 31 + xs[i]) & 2147483647;
    }
    delete[] xs;
    return total;
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
