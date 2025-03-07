// 生产者消费者模式
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
class ProducerConsumer {
private:
  std::queue<int> buffer;
  const int max_size = 10;
  std::mutex mtx;
  // 条件变量 生产者
  std::condition_variable cvProducer;
  // 条件变量 消费者
  std::condition_variable cvConsumer;

public:
  void produce(int data) {
    while (true) {
      // 获取锁
      std::unique_lock<std::mutex> lock(mtx);
      // 条件变量的wait函数会自动释放锁，并进入等待状态，直到被通知唤醒
      cvProducer.wait(lock, [this] { return buffer.size() < max_size; });
      buffer.push(data);
      std::cout << "生产数据: " << data << " | 队列大小: " << buffer.size()
                << std::endl;
      lock.unlock();
      cvConsumer.notify_one();
    }
  }

  void consume() {
    while (true) {
      std::unique_lock<std::mutex> lock(mtx);
      cvConsumer.wait(lock, [this] { return buffer.size() > 0; });
      int data = buffer.front();
      buffer.pop();
      std::cout << "消费数据: " << data << " | 队列大小: " << buffer.size()
                << std::endl;
      lock.unlock();
      cvProducer.notify_one();
    }
  }
};

// 生产者线程任务
void producer_task(ProducerConsumer &pc) {
  for (int i = 1; i <= 20; ++i) {
    pc.produce(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 模拟生产耗时
  }
}

// 消费者线程任务
void consumer_task(ProducerConsumer &pc) {
  for (int i = 1; i <= 20; ++i) {
    pc.consume();
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟消费耗时
  }
}

int main() {
  ProducerConsumer pc;
  std::thread producer(producer_task, std::ref(pc));
  std::thread producer2(producer_task, std::ref(pc));
  std::thread consumer(consumer_task, std::ref(pc));
  // 再加一个消费者
  std::thread consumer2(consumer_task, std::ref(pc));
  producer.join();
  consumer.join();

  return 0;
}
