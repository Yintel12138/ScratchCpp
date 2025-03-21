#include <iostream>

class MyClass {
public:
  int *data;
  // 移动构造函数
  MyClass(MyClass &&other) noexcept : data(other.data) {

    other.data = nullptr; // 转移资源后置空源对象
    // 输出other的地址
    std::cout << "&other: " << &other << std::endl;
  }
  MyClass() = default;
};

int main() {
  int a = 10;
  const int &&b = std::move(a);
  std::cout << "a: " << a << std::endl;
  // 打印a的地址
  std::cout << "&a: " << &a << std::endl;
  // 打印b的值
  std::cout << "b: " << b << std::endl;
  // 打印b的地址
  std::cout << "&b: " << &b << std::endl;
  MyClass c;
  // 打印c的地址
  std::cout << "&c: " << &c << std::endl;
  MyClass d(std::move(c));
  // 打印d的地址
  std::cout << "&d: " << &d << std::endl;
  // 打印c的data成员变量
  std::cout << "c.data: " << &c.data << std::endl;
  // 打印d的data成员变量
  std::cout << "d.data: " << &d.data << std::endl;

  return 0;
}