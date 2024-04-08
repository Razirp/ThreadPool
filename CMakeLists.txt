cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

# Set the include directories
include_directories(include)

# Add the source files
set(SOURCES src/thread_pool.cpp src/worker_thread.cpp)

# Generate the shared library
add_library(ThreadPool SHARED ${SOURCES})

# Set the output directory for the library
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set_target_properties(ThreadPool PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(ThreadPoolTest tests/test_thread_pool.cpp)

target_link_libraries(ThreadPoolTest ThreadPool)

# Set the output directory for the test executable
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_target_properties(ThreadPoolTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add the test executable

add_test(NAME ThreadPoolTest COMMAND ThreadPoolTest)

# Add the test executable to the list of tests

enable_testing()