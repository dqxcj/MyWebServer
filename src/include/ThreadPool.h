#ifndef SRC_INCLUDE_THREADPOOL_H_
#define SRC_INCLUDE_THREADPOOL_H_
#include <unistd.h>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(int thread_num) : stop_(false) {
    for (int i = 0; i < thread_num; i++) {
      threads_.emplace_back(std::thread([this, i] {
        // std::cout << i << "is start" << std::endl;
        while (true) {
          // std::cout << "I'm work thread " << i << std::endl;
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->mutex_);
            this->cond_var_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
            // std::cout << "I'm work thread " << i << std::endl;
            if (this->stop_ && this->tasks_.empty()) {
              // std::cout << i << "is close" << std::endl;
              return;
            }
            task = std::move(this->tasks_.front());
            this->tasks_.pop_front();
          }
          // std::cout << "I'm work thread " << i << std::endl;
          task();
        }
      }));
    }
  }

  ~ThreadPool() {
#ifndef THREADPOOLCLOSE
    Close();
#endif
  }

  void Close() {
#define THREADPOOLCLOSE
    // std::cout << "~ThreadPool" << std::endl;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      stop_ = true;
    }
    cond_var_.notify_all();
    std::thread([this] {
      while (true) {
        // std::cout << "I'm close thread" << std::endl;
        cond_var_.notify_all();
        sleep(1);
      }
    }).detach();
    for (int i = 0; i < threads_.size(); i++) {
      threads_[i].join();
      // std::cout << "thread " << i << " is over" << std::endl;
    }
    // std::cout << "over" << std::endl;
  }

  template <typename F, typename... Args>
  auto AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using return_type = decltype(f(args...));
    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(mutex_);

      // don't allow enqueueing after stopping the pool
      if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks_.emplace_back([task]() { (*task)(); });
    }
    cond_var_.notify_one();
    return res;
  }

 private:
  std::vector<std::thread> threads_;
  std::deque<std::function<void()>> tasks_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
  bool stop_;
};

#endif  // SRC_INCLUDE_THREADPOOL_H_
