// 手写shared_ptr

// 使用原子类型实现引用计数
#include <atomic>
#include <iostream>
template <typename T> class MySharedptr {
private:
  // 引用计数
  std::atomic_int *_count;
  // 原始指针
  T *_ptr;

public:
  MySharedptr()
      : _ptr(nullptr), _count(new std::atomic_int(0)){

                       };
  explicit MySharedptr(T *ptr) : _ptr(ptr), _count(new std::atomic_int(1)) {

    // 判断是否为空指针
    if (_ptr == nullptr) {
      _count = new std::atomic_int(0);
    }
  }

  MySharedptr(const MySharedptr &ptr) : _ptr(ptr._ptr), _count(ptr._count) {
    if (_ptr != nullptr) {
      (*_count)++;
    }
  }

  MySharedptr &operator=(const MySharedptr &ptr) {
    if (this == &ptr) {
      return *this;
    }
    if (--(*_count) == 0) {
      delete _ptr;
      delete _count;
    }
    _ptr = ptr._ptr;
    _count = ptr._count;
    (*_count)++;
    return *this;
  };
  MySharedptr(MySharedptr &&ptr) noexcept : _ptr(ptr._ptr), _count(ptr._count) {
    ptr._ptr = nullptr;
    ptr._count = nullptr;
  };
  MySharedptr &operator=(MySharedptr &&ptr) {
    if (this == &ptr) {
      return *this;
    }
    if (--(*_count) == 0) {
      delete _ptr;
      delete _count;
    }
    _ptr = ptr._ptr;
    _count = ptr._count;
    ptr._ptr = nullptr;
    ptr._count = nullptr;
    return *this;
  };
  // 析构函数
  ~MySharedptr() {
    if (--(*_count) == 0) {
      delete _ptr;
      delete _count;
    }
  }
  T &operator*() const { return *_ptr; }
  T *operator->() const { return _ptr; }
  int use_count() const {
    // 考虑被move的情况
    if (_count == nullptr) {
      return 0;
    }
    return *_count;
  }
  // 获得原始指针
  T *get() const { return _ptr; }
};
void test_destructor() {
  std::cout << "开始测试析构..." << std::endl;
  {
    // 创建一个带计数输出的对象
    class TestObj {
    public:
      TestObj() { std::cout << "TestObj 构造" << std::endl; }
      ~TestObj() { std::cout << "TestObj 析构" << std::endl; }
    };

    std::cout << "创建第一个智能指针" << std::endl;
    MySharedptr<TestObj> p1(new TestObj());
    std::cout << "引用计数: " << p1.use_count() << std::endl;

    {
      std::cout << "创建第二个智能指针" << std::endl;
      MySharedptr<TestObj> p2 = p1;
      std::cout << "引用计数: " << p1.use_count() << std::endl;
    }

    // p2离开作用域，引用计数应减1
    std::cout << "第二个指针已析构" << std::endl;
    std::cout << "引用计数: " << p1.use_count() << std::endl;
  }
  // p1离开作用域，引用计数变为0，应该析构对象
  std::cout << "第一个指针已析构" << std::endl;
}
int main() {
  test_destructor();
  return 0;
}