#include <iostream>
#include <chrono>
#include "thread_pool.hpp"

int main() {
    // Create a thread pool object
    thread_utils::thread_pool pool(10);

    // Define the task to be executed by the thread pool
    auto task = []() {
        // Perform some computation or task here
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Submit the task to the thread pool multiple times
    for (int i = 0; i < 1000; i++) {
        pool.submit(task);
    }

    // Wait for all tasks to complete
    pool.wait();

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the elapsed time
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Print the performance result
    std::cout << "Elapsed time: " << duration << " milliseconds" << std::endl;

    // Compare with the performance of executing tasks sequentially
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
        task();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sequential execution time: " << duration << " milliseconds" << std::endl;

    return 0;
}