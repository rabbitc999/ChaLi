#include "native_lsp_budget.hpp"
#include "native_lsp_scheduler.hpp"
#include <cassert>
#include <atomic>

/**
 * @brief 有界等待测试线程到达约定状态。
 * @param ready 状态谓词。
 * @return 无；两秒内未到达则断言失败。
 */
static void waitFor(const std::function<bool()> &ready)
{
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!ready() && std::chrono::steady_clock::now() < end) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    assert(ready());
}

/**
 * @brief 验证预算、交互优先、取消、队列背压及关闭回收。
 * @return 所有断言通过时为零。
 */
int main()
{
    LspAnalysisBudget budget(1);
    auto held = budget.acquire(false, [] { return false; });
    std::atomic<int> sequence{ 0 }, foregroundOrder{ 0 }, backgroundOrder{ 0 };
    std::atomic<bool> backgroundStarted{ false };
    std::thread background([&] {
        backgroundStarted.store(true);
        auto lease = budget.acquire(false, [] { return false; });
        backgroundOrder = ++sequence;
    });
    waitFor([&] { return backgroundStarted.load(); });
    std::thread foreground([&] {
        auto lease = budget.acquire(true, [] { return false; });
        foregroundOrder = ++sequence;
    });
    waitFor([&] { return budget.state().interactive == 1; });
    held.reset();
    foreground.join();
    background.join();
    assert(foregroundOrder == 1 && backgroundOrder == 2 && budget.state().active == 0 && budget.state().peak == 1);

    held = budget.acquire(false, [] { return false; });
    std::atomic<bool> cancel{ false }, returned{ false };
    std::thread waiter([&] { auto lease = budget.acquire(true, [&] { return cancel.load(); }); assert(!lease); returned = true; });
    waitFor([&] { return budget.state().interactive == 1; });
    cancel = true;
    waitFor([&] { return returned.load(); });
    waiter.join();
    held.reset();
    assert(!budget.state().active && !budget.state().interactive);

    LspAnalysisBudget parallel(2);
    auto first = parallel.acquire(true, [] { return false; });
    auto second = parallel.acquire(true, [] { return false; });
    assert(parallel.state().active == 2 && parallel.state().peak == 2);
    assert(!parallel.acquire(true, [] { return true; }));
    first.reset();
    second.reset();

    LspQueue queue;
    auto active = std::make_shared<std::atomic<bool>>(false);
    std::atomic<bool> running{ false };
    std::atomic<int> handled{ 0 }, cancelled{ 0 };
    queue.post("active", [&] {
        running = true;
        while (!active->load()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ++handled; }, active);
    waitFor([&] { return running.load(); });
    for (int i = 0; i < 500; ++i)
    {
        auto flag = std::make_shared<std::atomic<bool>>(false);
        queue.post("", [&, flag] { ++handled; if (flag->load()) ++cancelled; }, flag, 0, 2 * 1024 * 1024);
        assert(queue.pending().first <= 8 && queue.pending().second <= 16 * 1024 * 1024);
    }
    assert(cancelled == 492);
    auto large = std::make_shared<std::atomic<bool>>(false);
    queue.post("large", [&, large] { ++handled; if (large->load()) ++cancelled; }, large, 0, 32 * 1024 * 1024);
    assert(queue.pending().first == 1);
    auto small = std::make_shared<std::atomic<bool>>(false);
    queue.post("small", [&, small] { ++handled; if (small->load()) ++cancelled; }, small, 0, 1);
    assert(large->load() && queue.pending().second == 1);
    assert(!queue.drainUntil(std::chrono::steady_clock::now() + std::chrono::milliseconds(10)));
    queue.stop();
    assert(handled == 503 && cancelled == 502 && queue.pending().first == 0 && queue.pending().second == 0);
    auto late = std::make_shared<std::atomic<bool>>(false);
    queue.post("late", [&, late] { assert(late->load()); ++handled; }, late);
    assert(handled == 504 && !queue.pending().first);
    return 0;
}
