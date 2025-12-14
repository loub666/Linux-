#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>

class ThreadPool {
public:
    // 定义任务类型
    using Task = std::function<void()>;

    // 构造函数：指定线程数量
    explicit ThreadPool(size_t num_threads) : running(false) {
        start(num_threads);
    }

    // 析构函数
    ~ThreadPool() {
        stop();
    }

    // 添加任务
    void enqueue(Task task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.emplace(std::move(task));
        }
        condition_.notify_one();
    }

private:
    // 工作线程函数
    void worker() {
        while (running) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                // 等待任务或停止信号
                condition_.wait(lock, [this] { return !running || !tasks_.empty(); });

                // 如果停止且队列空，退出
                if (!running && tasks_.empty()) return;

                // 取出任务
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            // 执行任务
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "ThreadPool task error: " << e.what() << std::endl;
            }
        }
    }

    // 启动线程池
    void start(size_t num_threads) {
        if (num_threads == 0) num_threads = 1;
        running = true;
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back(&ThreadPool::worker, this);
        }
    }

    // 停止线程池
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running = false;
        }
        condition_.notify_all(); // 唤醒所有线程让它们退出

        for (std::thread& t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    std::vector<std::thread> threads_;
    std::queue<Task> tasks_;
    
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running;
};

#endif // THREAD_POOL_H
