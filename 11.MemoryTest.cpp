#include <iostream>
#include <new> // for std::bad_alloc

int main() {
  try {
    // 尝试分配大量内存
    size_t size = 1000000000; // 1GB
    int *largeArray = new int[size];

    // 使用内存
    for (size_t i = 0; i < size; ++i) {
      largeArray[i] = i;
    }

    // 释放内存
    delete[] largeArray;
  } catch (const std::bad_alloc &e) {
    std::cerr << "Memory allocation failed: " << e.what() << std::endl;
    // 处理内存分配失败的情况
    // 例如，减少内存分配量或释放其他内存
  }

  return 0;
}
