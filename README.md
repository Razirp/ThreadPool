# C++ Thread Pool Library

A high-performance thread pool implementation in C++ for executing tasks concurrently.

## Introduction

The C++ Thread Pool Library is designed to manage and execute tasks concurrently using a pool of worker threads. It provides a simple and efficient way to distribute tasks across multiple threads, improving the overall performance and responsiveness of your application.

## Features

- Dynamically manages a pool of worker threads.
- Supports task pausing, resuming, and termination.
- Allows setting a maximum limit on the number of tasks in the queue.
- Thread-safe operations using `std::shared_mutex` and `std::condition_variable`.
- Exception handling for task execution.

## Installation

### From Source

To build the thread pool library from source, follow these steps:

1. Clone the repository:
   ```
   git clone https://github.com/Razirp/ThreadPool.git
   ```
2. Navigate to the project directory:
   ```
   cd thread-pool-library
   ```
3. Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```
4. Run `cmake` to generate the build files:
   ```
   cmake ..
   ```
5. Compile the library:
   ```
   cmake --build .
   ```

### Using Precompiled Binaries

Precompiled binaries for Windows and Linux are available in the [Releases](https://github.com/your-username/thread-pool-library/releases) section. Download the appropriate archive for your platform and extract it to a location of your choice.

## Usage

To use the thread pool library, include the `thread_pool.hpp` header file in your project and follow the example below:

```cpp
#include "thread_pool.hpp"
#include <iostream>
#include <future>

void task(int id) {
    std::cout << "Hello from thread " << id << std::endl;
}

int main() {
    thread_utils::thread_pool pool(4); // Create a pool with 4 threads
    for (int i = 0; i < 8; ++i) {
        pool.submit(task, i);
    }
    pool.terminate(); // Wait for all tasks to complete and terminate the pool
    return 0;
}
```

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any questions or inquiries, please reach out to me at RenPengyu2001@outlook.com.