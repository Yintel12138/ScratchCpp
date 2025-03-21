#include <iostream>
class Base {};
class Derived : public Base {};
void legacy_api(int *ptr){};

int main() {
  // static_cast
  double a = 3.14;
  int i = static_cast<int>(a);
  std::cout << i << std::endl;
  // 类层次上行转换
  Derived d;
  Base *pb = static_cast<Base *>(&d);
  // 类层次下行转换
  Base *pb2 = new Base;
  Derived *pd = static_cast<Derived *>(pb2);

  // dynamic_cast
  // 用于多态运行时的安全转换
  
  // const_cast
  const int k = 42;
  int *j = const_cast<int *>(&k);
  std::cout << *j << std::endl;
  *j = 43;
  std::cout << k << std::endl;
  std::cout << *j << std::endl;
  // 调用历史遗留非const接口
  const int data = 100;
  legacy_api(const_cast<int *>(&data));

  // reinterpret_cast
  // 通常用于位操作
  int *ptr = new int(0x1234);
  uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
  std::cout << std::hex << addr << std::endl;

  float f = 3.14f;
  int *i_ptr = reinterpret_cast<int *>(&f);
  std::cout << std::hex << *i_ptr << std::endl;
  return 0;
}