#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <utility>

/**
 * @brief 一个提示类别的有界工作队列；协议线程只负责提交和取消。
 */
class LspQueue
{
    struct Work
    {
        std::string key;
        std::function<void()> run;
        std::shared_ptr<std::atomic<bool>> cancelled;
        std::chrono::steady_clock::time_point due;
        size_t bytes;
    };
    std::mutex mutex_;
    std::condition_variable changed_, idle_;
    std::deque<Work> work_;
    bool stopping_ = false, active_ = false;
    std::thread thread_;
    size_t pendingBytes_ = 0;
    std::shared_ptr<std::atomic<bool>> activeCancellation_;
    /** @brief 执行到期任务，取消处理同样在锁外完成。 */
    void loop()
    {
        for (;;)
        {
            Work item;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                for (;;)
                {
                    if (stopping_ && work_.empty()) return;
                    if (work_.empty())
                    {
                        changed_.wait(lock);
                        continue;
                    }
                    auto next = std::min_element(work_.begin(), work_.end(), [](const Work &a, const Work &b) { return a.due < b.due; });
                    if (!stopping_ && next->due > std::chrono::steady_clock::now())
                    {
                        const auto due = next->due;
                        changed_.wait_until(lock, due);
                        continue;
                    }
                    item = std::move(*next);
                    pendingBytes_ -= item.bytes;
                    work_.erase(next);
                    active_ = true;
                    activeCancellation_ = item.cancelled;
                    break;
                }
            }
            try
            {
                item.run();
            } catch (...) { /* 单次任务异常由请求入口报告；线程继续服务。 */ }
            {
                std::lock_guard<std::mutex> lock(mutex_);
                active_ = false;
                activeCancellation_.reset();
                if (work_.empty()) idle_.notify_all();
            }
        }
    }

public:
    /**
     * @brief 启动一个独立提示线程。
     */
    LspQueue() : thread_([this] { loop(); }) {}
    /**
     * @brief 等待队列任务结束并释放线程。
     */
    ~LspQueue() { stop(); }
    /**
     * @brief 提交最新任务，相同键替换旧任务；队列最多保留 128 项。
     * @param key 合并键。
     * @param run 执行函数。
     * @param cancelled 取消标记。
     * @param delay 防抖时间。
     * @param bytes 快照保守估计字节数，每通道最多排队 16 MiB；空队列允许单个更大的快照。
     */
    void post(std::string key, std::function<void()> run, std::shared_ptr<std::atomic<bool>> cancelled, int delay = 0, size_t bytes = 0)
    {
        std::vector<Work> removed;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_)
            {
                cancelled->store(true);
                removed.push_back({ std::move(key), std::move(run), std::move(cancelled), {}, bytes });
            }
            else
            {
                for (auto it = work_.begin(); it != work_.end();)
                {
                    if (!key.empty() && it->key == key)
                    {
                        it->cancelled->store(true);
                        pendingBytes_ -= it->bytes;
                        removed.push_back(std::move(*it));
                        it = work_.erase(it);
                    }
                    else
                        ++it;
                }
                while (!work_.empty() && (work_.size() >= 128 || bytes > 16ULL * 1024 * 1024 || pendingBytes_ > 16ULL * 1024 * 1024 - bytes))
                {
                    work_.front().cancelled->store(true);
                    pendingBytes_ -= work_.front().bytes;
                    removed.push_back(std::move(work_.front()));
                    work_.pop_front();
                }
                work_.push_back({ std::move(key), std::move(run), std::move(cancelled), std::chrono::steady_clock::now() + std::chrono::milliseconds(delay), bytes });
                pendingBytes_ += bytes;
            }
        }
        for (auto &item : removed) item.run();
        changed_.notify_one();
    }
    /**
     * @brief 等待已提交的请求全部完成，用于 JSON-RPC shutdown 屏障。
     */
    void drain()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        idle_.wait(lock, [&] { return !active_ && work_.empty(); });
    }
    /**
     * @brief 在统一关闭期限前等待排队任务完成。
     * @param deadline 关闭期限。
     * @return 已全部完成时为 true。
     */
    bool drainUntil(std::chrono::steady_clock::time_point deadline)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return idle_.wait_until(lock, deadline, [&] { return !active_ && work_.empty(); });
    }
    /**
     * @brief 查询排队快照占用，活跃任务不计入排队预算。
     * @return 排队项数及估计字节数。
     */
    std::pair<size_t, size_t> pending()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return { work_.size(), pendingBytes_ };
    }
    /**
     * @brief 移除尚未开始的取消请求，立即释放其文档快照并回送取消响应。
     * @param flag 请求的取消标记。
     */
    void cancel(const std::shared_ptr<std::atomic<bool>> &flag)
    {
        std::vector<Work> removed;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto it = work_.begin(); it != work_.end();)
            {
                if (it->cancelled == flag)
                {
                    pendingBytes_ -= it->bytes;
                    removed.push_back(std::move(*it));
                    it = work_.erase(it);
                }
                else
                    ++it;
            }
            if (!active_ && work_.empty()) idle_.notify_all();
        }
        for (auto &item : removed) item.run();
        changed_.notify_one();
    }
    /**
     * @brief 停止调度并等待工作线程。
     */
    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
            if (activeCancellation_) activeCancellation_->store(true);
            for (auto &item : work_) item.cancelled->store(true);
        }
        changed_.notify_all();
        if (thread_.joinable()) thread_.join();
    }
};
