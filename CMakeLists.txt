cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建模式
set(CMAKE_BUILD_TYPE Release)
# 设置 C 编译器的优化等级
set(CMAKE_C_FLAGS_RELEASE "-O2") # 为 Release 类型设置 -O2 优化
set(CMAKE_C_FLAGS_DEBUG "-g")    # 为 Debug 类型设置 -g（生成调试信息），不设置优化

# 设置 C++ 编译器的优化等级
set(CMAKE_CXX_FLAGS_RELEASE "-O2") # 为 Release 类型设置 -O2 优化
set(CMAKE_CXX_FLAGS_DEBUG "-g")    # 为 Debug 类型设置 -g（生成调试信息），不设置优化

# Set the include directories
include_directories(include)

# Add the source files
set(SOURCES src/thread_pool.cpp src/worker_thread.cpp)

# Set the output directory for the library
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Generate the shared library
add_library(ThreadPool SHARED ${SOURCES})

add_executable(FunctionalTest tests/functional_test.cpp)
add_executable(PerformanceTest tests/performance_test.cpp)

target_link_libraries(FunctionalTest PRIVATE ThreadPool)
target_link_libraries(PerformanceTest PRIVATE ThreadPool)

set_target_properties(ThreadPool PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
set_target_properties(FunctionalTest PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_target_properties(PerformanceTest PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_test(NAME FunctionalTest COMMAND FunctionalTest)
add_test(NAME PerformanceTest COMMAND PerformanceTest)
enable_testing()