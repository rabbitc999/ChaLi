#pragma once
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

/** @brief 语义子进程预算；交互请求先于等待中的后台诊断获得空位。 */
class LspAnalysisBudget
{
    std::mutex mutex_;
    std::condition_variable changed_;
    size_t limit_, active_ = 0, peak_ = 0, interactive_ = 0;
    /** @brief 归还一个空位并唤醒等待者。 */
    void release()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        --active_;
        changed_.notify_all();
    }

public:
    /** @brief 持有一个子进程空位，析构时归还。 */
    class Lease
    {
        friend class LspAnalysisBudget;
        LspAnalysisBudget *owner_ = nullptr;

    public:
        /** @brief 创建尚未获得空位的租约。 */
        Lease() = default;
        /** @brief 禁止复制租约，避免重复归还同一个空位。 */
        Lease(const Lease &) = delete;
        /** @brief 禁止赋值租约，保持空位的唯一持有者。 */
        Lease &operator=(const Lease &) = delete;
        /** @brief 归还已获得的空位。 */
        ~Lease()
        {
            if (owner_) owner_->release();
        }
    };
    /** @brief 当前预算及交互等待数量。 */
    struct State
    { size_t active, peak, interactive; };
    /**
     * @brief 创建有界预算。
     * @param limit 并行上限，至少为一。
     */
    explicit LspAnalysisBudget(size_t limit = 2) : limit_(std::max<size_t>(1, limit)) {}
    /**
     * @brief 等待空位；取消时不启动子进程。
     * @param foreground 是否为交互请求。
     * @param cancelled 无异常的取消检查。
     * @return 已获得的租约，取消时为空。
     */
    std::unique_ptr<Lease> acquire(bool foreground, const std::function<bool()> &cancelled)
    {
        auto lease = std::make_unique<Lease>();
        std::unique_lock<std::mutex> lock(mutex_);
        if (foreground) ++interactive_;
        while (!cancelled() && (active_ >= limit_ || (!foreground && interactive_)))
            changed_.wait_for(lock, std::chrono::milliseconds(20));
        if (foreground) --interactive_;
        changed_.notify_all();
        if (cancelled()) return {};
        ++active_;
        peak_ = std::max(peak_, active_);
        lease->owner_ = this;
        return lease;
    }
    /**
     * @brief 读取预算统计。
     * @return 活跃、峰值及等待中的交互请求数。
     */
    State state()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return { active_, peak_, interactive_ };
    }
};
