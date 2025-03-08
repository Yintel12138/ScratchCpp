// 6. 手写线程池
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
// 成员变量
class ThreadPool {
private:
  // 创建一个队列
  std::queue<std::function<void()>> tasks;
  // 创建一个互斥锁
  std::mutex mtx;
  // 创建一个条件变量
  std::condition_variable cv;
  // 线程池是否关闭
  bool stop;
  // 创建线程池
  std::vector<std::thread> workers;

  void workerThread() {
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return stop || !tasks.empty(); });
        if (stop && tasks.empty()) {
          return;
        }
        task = tasks.front();
        tasks.pop();
      }
      task();
    }
  }

public:
  // 需要实现的接口
  explicit ThreadPool(size_t threadNum) : stop(false) {
    for (size_t i = 0; i < threadNum; ++i) {
      workers.emplace_back([this] { this->workerThread(); });
    };
  }
  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(mtx);
      stop = true;
    }
    cv.notify_all();
    for (auto &t : workers) {
      if (t.joinable())
        t.join();
    }
  };
  template <typename F, typename... Args> void addTask(F &&f, Args &&...args) {
    // 加入队列
    {
      std::unique_lock<std::mutex> lock(mtx);
      tasks.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }
    cv.notify_one();
  }
};

int main() {}