cmake_minimum_required(VERSION 3.20)
project(test_thread_pool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/../include)

set(SOURCE_FILES
    test_thread_pool.cpp
)

add_executable(test_thread_pool ${SOURCE_FILES})
