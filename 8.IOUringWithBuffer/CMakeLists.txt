# CMakeLists.txt for IOUring with Buffer project

# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称和语言
project(IOUringWithBuffer VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译类型，如果未指定则设为 Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# 添加源文件
set(SOURCES
    Buffer.cpp
    main.cpp
    IOUringHandler.cpp
)

# 添加头文件
set(HEADERS
    Buffer.h
    IOUringHandler.h
)

# 生成可执行文件
add_executable(io_uring_buffer ${SOURCES} ${HEADERS})

# 查找 liburing 库
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(LIBURING liburing)
  if(LIBURING_FOUND)
    target_include_directories(io_uring_buffer PRIVATE ${LIBURING_INCLUDE_DIRS})
    target_link_libraries(io_uring_buffer PRIVATE ${LIBURING_LIBRARIES})
  else()
    # 尝试直接使用 uring 库
    find_library(LIBURING_LIB uring)
    if(LIBURING_LIB)
      target_link_libraries(io_uring_buffer PRIVATE uring)
    else()
      message(FATAL_ERROR "liburing not found. Please install liburing-dev package.")
    endif()
  endif()
else()
  # 尝试直接使用 uring 库
  find_library(LIBURING_LIB uring)
  if(LIBURING_LIB)
    target_link_libraries(io_uring_buffer PRIVATE uring)
  else()
    message(FATAL_ERROR "liburing not found. Please install liburing-dev package.")
  endif()
endif()

# 增加可选的测试支持
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# 安装目标
install(TARGETS io_uring_buffer
  RUNTIME DESTINATION bin
)

# 输出配置信息
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ flags (${CMAKE_BUILD_TYPE}): ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")