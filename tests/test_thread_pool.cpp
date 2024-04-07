#include <iostream>
#include <cassert>
#include "thread_pool.hpp"

// Test function
int add(int a, int b) {
    return a + b;
}

// Test function that throws an exception
void throw_exception() {
    throw std::runtime_error("Test exception");
}

int main() {
    // Create a thread pool with 4 threads
    thread_utils::thread_pool pool(4);

    // Test submitting a task and getting the result
    auto future = pool.submit(add, 2, 3);
    assert(future.get() == 5);
    std::cout << "Test 1 passed!" << std::endl;

    // Test submitting multiple tasks
    auto future1 = pool.submit(add, 4, 5);
    auto future2 = pool.submit(add, 6, 7);
    assert(future1.get() == 9);
    assert(future2.get() == 13);
    std::cout << "Test 2 passed!" << std::endl;

    // Test submitting tasks that throw exceptions
    auto future3 = pool.submit(throw_exception);
    try {
        future3.get();
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "Test exception");
    }
    std::cout << "Test 3 passed!" << std::endl;

    // Test pausing and resuming the thread pool
    pool.pause();
    try {
        pool.submit(add, 8, 9); // Should throw exception because pool is paused
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "[thread_pool::submit][error]: thread pool is paused");
    }
    pool.resume();
    auto future4 = pool.submit(add, 8, 9);
    assert(future4.get() == 17);
    std::cout << "Test 4 passed!" << std::endl;

    // Test adding and removing threads from the pool
    pool.add_thread(2);
    assert(pool.get_thread_count() == 6);
    pool.remove_thread(3);
    assert(pool.get_thread_count() == 3);
    std::cout << "Test 5 passed!" << std::endl;

    while (pool.get_task_count() > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Test setting the maximum task count
    pool.set_max_task_count(2);
    auto future5 = pool.submit(add, 10, 11);
    auto future6 = pool.submit(add, 12, 13);
    try {
        pool.submit(add, 14, 15); // Should throw exception because task queue is full
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "[thread_pool::submit][error]: task queue is full");
    }
    std::cout << "Test 6 passed!" << std::endl;

    // Test terminating the thread pool
    pool.terminate();
    try {
        pool.submit(add, 16, 17); // Should throw exception because pool is terminated
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "[thread_pool::submit][error]: thread pool is terminated");
    }
    std::cout << "Test 7 passed!" << std::endl;

    std::cout << "All tests passed!" << std::endl;

    return 0;
}