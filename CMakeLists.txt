# 设置编译flag
# DEBUG
set(CMAKE_BUILD_TYPE "DEBUG")

# 编译
add_executable(1.MyVector 1.MyVector.cpp)
add_executable(2.MySharedptr 2.MySharedptr.cpp)
add_executable(3.ProduceAndConsumer 3.ProduceAndConsumer.cpp)
add_executable(4.LRUCache 4.LRUCache.cpp)
add_executable(5.Singleton 5.Singleton.cpp)
add_executable(6.ThreadPool 6.ThreadPool.cpp)

# 配置运行的文件
