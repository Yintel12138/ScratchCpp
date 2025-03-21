# 设置编译flag
# DEBUG
cmake_minimum_required(VERSION 3.10)
project(Project)
set(CMAKE_BUILD_TYPE "DEBUG")

# 编译
add_executable(0.CodeTest 0.CodeTest.cpp)
add_executable(1.MyVector 1.MyVector.cpp)
add_executable(2.MySharedptr 2.MySharedptr.cpp)
add_executable(3.ProduceAndConsumer 3.ProduceAndConsumer.cpp)
add_executable(4.LRUCache 4.LRUCache.cpp)
# add_executable(5.Singleton 5.Singleton.cpp)
add_executable(6.ThreadPool 6.ThreadPool.cpp)
add_executable(7.NickIOUring 7.NickIOUring.cpp)
add_subdirectory(8.IOUringWithBuffer)
add_executable(9.Cast 9.Cast.cpp)
add_subdirectory(10.Kafka)


find_library(LIBURING_LIB uring)
target_link_libraries(7.NickIOUring PRIVATE uring)
# 配置运行的文件
