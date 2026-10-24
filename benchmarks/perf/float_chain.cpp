/* float_chain —— 依赖链式浮点乘加（双精度），1000000 轮。C++17，g++ -O2。 */
#include <chrono>
#include <cstdint>
#include <cstdio>

namespace
{
constexpr std::int64_t N = 1000000;
constexpr int REPS = 5;

/**
 * @brief 执行依赖前值的双精度浮点乘加循环，生成跨语言校验值。
 * @param seed 各次测量的不同初值，防止跨轮常量复用。
 * @return 计算结果，供计时入口归并校验。
 */
double work(int64_t seed)
{
    double x = static_cast<double>(seed);
    for (int64_t i = 0; i < N; i++)
    {
        x = x * 1.0000001 + 0.0000001;
    }
    return x;
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
    double (*volatile kernel)(int64_t) = work;
    volatile double sink = kernel(0);
    for (int r = 0; r < REPS; ++r)
    {
        const std::int64_t t0 = now_ns();
        sink = sink * 0.5 + kernel(r);
        const std::int64_t t1 = now_ns();
        std::printf("rep %lld\n", static_cast<long long>(t1 - t0));
    }
    std::printf("sink %.9f\n", (double)sink);
    return 0;
}
