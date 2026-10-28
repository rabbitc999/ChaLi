/* map_growth：默认标准哈希容器，不预留容量。 */
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <unordered_map>
constexpr std::int64_t N=20000;
constexpr int REPS=5;
/**
 * @brief 构造分散且唯一的正整数键，避开 Lua table 的连续数组区。
 * @param index 操作顺序。
 * @param seed 本轮初值。
 * @return 位于十亿附近的整数键。
 */
static std::int64_t key(std::int64_t index,std::int64_t seed)
{ return (index*65537+seed*17)%1000000007+1000000007; }
/**
 * @brief 动态增长哈希集合，执行确定次序的修改与查询。
 * @param seed 本轮初值。
 * @return 含查询与删除结果的有序校验值。
 */
static std::int64_t work(std::int64_t seed)
{
    std::int64_t total=seed;
    std::unordered_map<std::int64_t,std::int64_t> values;
    for (std::int64_t i=0;i<N;++i) values[key(i,seed)]=i*3+seed;
    for (std::int64_t i=0;i<N;++i) {
        auto k=key(i,seed);
        total=(total*31+values.at(k))&2147483647;
        values[k]=i*5+seed;
    }
    for (std::int64_t i=0;i<N;++i) {
        auto k=key(i,seed);
        total=(total*31+values.at(k)+static_cast<std::int64_t>(values.count(-k)))&2147483647;
    }
    for (std::int64_t i=0;i<N;i+=3) total+=values.erase(key(i,seed));
    for (std::int64_t i=0;i<N;i+=3) values[key(i,seed)]=i*7+seed;
    for (std::int64_t i=0;i<N;++i) total=(total*31+values.at(key(i,seed)))&2147483647;
    return total;
}
/**
 * @brief 读取宿主单调时钟。
 * @return 当前单调时间的纳秒读数。
 */
static long long now_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

/**
 * @brief 预热后通过易变函数入口计时，归并跨语言校验值。
 * @return 完成五次测量后返回零。
 */
int main()
{
    std::int64_t (*volatile kernel)(std::int64_t) = work;
    volatile std::int64_t sink = kernel(0);
    for (int r = 0; r < REPS; ++r)
    {
        long long start = now_ns();
        sink += kernel(r);
        long long elapsed = now_ns() - start;
        std::printf("rep %lld\n", elapsed);
    }
    std::printf("sink %lld\n", (long long)sink);
    return 0;
}
