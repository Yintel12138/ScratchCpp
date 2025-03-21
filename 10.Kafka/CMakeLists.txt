cmake_minimum_required(VERSION 3.11)
project(KafkaModule)

# 使用find_package查找RdKafka
find_package(PkgConfig REQUIRED)
pkg_check_modules(RdKafka REQUIRED IMPORTED_TARGET rdkafka++>=1.0.0)

# 如果rdkafka安装在非标准位置，可保留这行
include_directories(/usr/include/librdkafka)

# 添加可执行文件
add_executable(KafkaProducer 10.KafkaProducer.cpp)
add_executable(KafkaConsumer 10.KafkaConsumer.cpp)

# 统一配置函数
function(configure_kafka_target target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_link_libraries(${target} PRIVATE 
        PkgConfig::RdKafka
        pthread
    )
endfunction()

configure_kafka_target(KafkaProducer)
configure_kafka_target(KafkaConsumer)