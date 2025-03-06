// 手写单例模式

// 饿汉模式
class Singleton {
private:
  Singleton() = default;
  static Singleton *instance;

public:
  static Singleton &getInstance() { return *instance; }
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;
};
// 初始化,线程安全
Singleton *Singleton::instance = new Singleton();

// 在需要的时候才创建对象
// 懒汉模式
class SingletonLazy {
private:
  static SingletonLazy *instance;
  SingletonLazy() = default;

public:
  SingletonLazy(const Singleton &) = delete;
  SingletonLazy &operator=(const Singleton &) = delete;
  static SingletonLazy &getInstance() {
    if (!instance) {
      instance = new SingletonLazy();
    }
    return *instance;
  }
};
// 初始化为null
SingletonLazy *SingletonLazy::instance = nullptr;

#include <mutex>

// 线程安全的懒汉模式
class SingletonLazyThreadSafe {
private:
  static SingletonLazyThreadSafe *instance;
  static std::mutex mtx;
  SingletonLazyThreadSafe() = default;

public:
  SingletonLazyThreadSafe(const SingletonLazyThreadSafe &) = delete;
  SingletonLazyThreadSafe &operator=(const SingletonLazyThreadSafe &) = delete;
  static SingletonLazyThreadSafe &getInstance() {
    // 加上互斥锁,生命周期结束后自动释放
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (!instance) {
        instance = new SingletonLazyThreadSafe();
      }
    }
    return *instance;
  }
};
// 初始化为null
SingletonLazyThreadSafe *SingletonLazyThreadSafe::instance = nullptr;

// 双重检查锁
class SingletonLazyDoubleCheck {
private:
  static SingletonLazyDoubleCheck *instance;
  static std::mutex mtx;
  SingletonLazyDoubleCheck() = default;

public:
  SingletonLazyDoubleCheck(const SingletonLazyDoubleCheck &) = delete;
  SingletonLazyDoubleCheck &
  operator=(const SingletonLazyDoubleCheck &) = delete;
  static SingletonLazyDoubleCheck &getInstance() {
    // 加上互斥锁,生命周期结束后自动释放,减少锁的开销
    // 只有在创建对象的时候才加锁
    if (!instance) {
      std::lock_guard<std::mutex> lock(mtx);
      if (!instance) {
        instance = new SingletonLazyDoubleCheck();
      }
    }
    return *instance;
  }
};
// 初始化为null
SingletonLazyDoubleCheck *SingletonLazyDoubleCheck::instance = nullptr;
std::mutex SingletonLazyDoubleCheck::mtx;

#include <atomic>
// 双重检查锁
class SingletonLazyAdvanceDoubleCheck {
private:
  static std::mutex mtx;
  SingletonLazyAdvanceDoubleCheck() = default;
  static std::atomic<SingletonLazyAdvanceDoubleCheck *> instance;

public:
  SingletonLazyAdvanceDoubleCheck(const SingletonLazyAdvanceDoubleCheck &) =
      delete;
  SingletonLazyAdvanceDoubleCheck &
  operator=(const SingletonLazyAdvanceDoubleCheck &) = delete;
  static SingletonLazyAdvanceDoubleCheck &getInstance() {
    // 加上互斥锁,生命周期结束后自动释放,减少锁的开销
    // 只有在创建对象的时候才加锁
    SingletonLazyAdvanceDoubleCheck *tmp =
        instance.load(std::memory_order_relaxed);
    if (!tmp) {
      std::lock_guard<std::mutex> lock(mtx);
      tmp = instance.load(std::memory_order_relaxed);
      if (!tmp) {
        tmp = new SingletonLazyAdvanceDoubleCheck();
        instance.store(tmp, std::memory_order_release);
      }
    }
    return *tmp;
  }
};
// 初始化为null
std::atomic<SingletonLazyAdvanceDoubleCheck *>
    SingletonLazyAdvanceDoubleCheck::instance{nullptr};
std::mutex SingletonLazyAdvanceDoubleCheck::mtx;

// C++11之后的线程安全的懒汉模式
class MeyersSingleton {
private:
  MeyersSingleton() = default;

public:
  static MeyersSingleton &getInstance() {
    static MeyersSingleton instance;
    return instance;
  }
  MeyersSingleton(const MeyersSingleton &) = delete;
  MeyersSingleton &operator=(const MeyersSingleton &) = delete;
};