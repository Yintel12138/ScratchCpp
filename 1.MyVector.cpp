// 手写实现vector的基本功能
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>
template <typename T> class myVector {
private:
  T *_data;
  int _capacity;
  int _size;

public:
  // 构造函数
  myVector<T>(int capacity = 10) : _capacity(capacity), _size(0) {
    // 如果传为负值
    if (capacity < 0) {
      throw std::invalid_argument("capacity < 0");
    }
    _data = new T[capacity];
  }
  // 拷贝构造函数
  myVector(const myVector &v) : _capacity(v._capacity), _size(v._size) {
    _data = new T[_capacity];
    std::move(v._data, v._data + _size, _data);
  }
  // 赋值运算符
  myVector &operator=(const myVector &v) {
    if (this == &v) {
      return *this;
    }
    _capacity = v._capacity;
    _size = v._size;
    delete[] _data;
    _data = new T[_capacity];
    std::move(v._data, v._data + _size, _data);
    return *this;
  }
  // 移动构造函数
  myVector(myVector &&v) {
    _capacity = v._capacity;
    _size = v._size;
    _data = v._data;
    v._data = nullptr;
    v._capacity = 0;
    v._size = 0;
  }
  // 移动赋值运算符
  myVector &operator=(myVector &&v) {
    if (this == &v) {
      return *this;
    }
    _capacity = v._capacity;
    _size = v._size;
    _data = v._data;
    v._data = nullptr;
    v._capacity = 0;
    v._size = 0;
    return *this;
  }
  // 析构函数
  ~myVector() { delete[] _data; }

  // 扩容
  void unsafeResize(int newCapacity) {
    T *newData = new T[newCapacity];
    for (int i = 0; i < _size; i++) {
      newData[i] = _data[i];
    }
    delete[] _data;
    _data = newData;
    _capacity = newCapacity;
  }
  // 实现安全扩容
  void resize(int newCapacity) {
    T *newData = new T[newCapacity];
    try {
      std::copy(_data, _data + _size, newData);
    } catch (...) {
      delete[] newData;
      throw;
    }
    delete[] _data;
    _data = newData;
    _capacity = newCapacity;
  }
  // push_back
  void push_back(T t) {
    // 添加元素到末尾
    // 查看是否需要扩容
    if (_size == _capacity) {
      resize(1.5 * _capacity);
    }
    _data[_size++] = t;
  }
  // front
  T &front() { return _data[0]; }
  // back
  T &back() { return _data[_size - 1]; }
  // size
  const int size() { return _size; }
  // capacity
  int capacity() { return _capacity; }
  // operator[]
  T &operator[](size_t index) { return _data[index]; }
  const T &operator[](size_t index) const { return _data[index]; }
};

int main() {
  myVector<int> v;
  v.push_back(1);
  v.push_back(2);
  for (int i = 0; i < v.size(); i++) {
    std::cout << v[i] << std::endl;
  }
  std::cout << "size: " << v.size() << std::endl;
  std::cout << "capacity: " << v.capacity() << std::endl;
  // 触发扩容
  for (int i = 0; i < 10; i++) {
    v.push_back(i);
  }

  std::cout << "size: " << v.size() << std::endl;
  std::cout << "capacity: " << v.capacity() << std::endl;
  // 测试front back
  std::cout << "front: " << v.front() << std::endl;
  std::cout << "back: " << v.back() << std::endl;

  // 测试拷贝构造函数
  myVector<int> v2(v);

  // 测试移动构造函数
  myVector<int> v3(std::move(v2));
  std::cout << "size: " << v3.size() << std::endl;
  std::cout << "capacity: " << v3.capacity() << std::endl;
  return 0;
}