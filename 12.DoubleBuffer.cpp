// 双缓冲队列实现

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
class DoubleBuffer {
private:
  static constexpr std::size_t BUFFER_SIZE = 1024;
  std::vector<std::string> _buffers[2];
  std::atomic<int> _currentBufferIndex{0};

  std::mutex _mutex;
  std::condition_variable _cv;
  std::atomic<bool> _running{true};

  // 文件输出
  std::ofstream _log_file;
  std::thread _thread;


  // 线程函数
  
public:
  // 构造函数
  DoubleBuffer(const std::string &filename) {
    _log_file.open(filename);
    // 判断是否打开成功
    if (!_log_file.is_open()) {
      throw std::runtime_error("open file failed");
    }
    // 启动线程
  }

  ~DoubleBuffer() {
    _running = false;
    _cv.notify_one();
    if (_thread.joinable()) {
      _thread.join();
    }
    _log_file.close();
  }

  // 写入日志
  void write(const std::string &log) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto &currentBuffer = _buffers[_currentBufferIndex];
    currentBuffer.push_back(log);
    if (currentBuffer.size() >= BUFFER_SIZE) {
      _cv.notify_one();
    }
  }
};