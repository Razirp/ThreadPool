C++ Thread Pool
==============

📖 **[[中文版本](README.md)]**

This is an implementation of a C++ thread pool based on modern C++, designed to efficiently execute concurrent tasks using a thread pool, thereby avoiding the overhead of frequently creating and destroying threads.

## Introduction

The C++ thread pool library is specifically tailored for concurrently managing and executing tasks using a set of working threads. It provides a simple yet efficient way to distribute tasks across multiple threads, significantly boosting the overall performance and responsiveness of your application.

## Features

- Dynamic management of a group of worker threads
- Support for pausing, resuming, and terminating tasks
- Ability to set a maximum limit on the number of tasks in the queue
- Use of `std::shared_mutex` and `std::condition_variable` for thread-safe operations
- Exception handling during task execution

## Project File Structure

```plaintext
ThreadPool/
├── README.md
├── README_CN.md		# Chinese README
├── LICENSE				# MIT LICENSE
├── .gitignore			# Git ignore file, specifying files/folders not to be version controlled
├── .github/workflows/	# GitHub Actions workflows
│	└── cmake-multi-platform.yml	# Configuration for GitHub Actions workflow
├── include/			# Header files directory
│   └── thread_pool.hpp		# Contains declarations for the thread pool class
├── src/				# Cpp files directory, containing implementations of non-template/inline member functions declared in the headers
│   ├── thread_pool.cpp		# Contains implementations of non-template/inline member functions for the thread pool class
│   └── worker_thread.cpp		# Contains implementations of non-template/inline member functions for the worker thread class
└── tests/				# Test files directory, containing various test programs
    ├── functional_test.cpp	# Test program for basic functionality of the thread pool
    └── performance_test.cpp	# Test program for the performance of the thread pool
```

## Installation Guide

#### Building from Source Code

To build the thread pool library from source, follow these steps:

1. Clone the repository (obtain the source code):  
   ```sh
   git clone https://github.com/Razirp/ThreadPool.git
   ```
   > Alternatively, you can use any method you're familiar with, such as cloning via SSH, using the GitHub CLI, or directly downloading a zip archive.

2. Enter the project directory:  
   ```sh
   cd ThreadPool
   ```
   > The directory name may vary depending on your naming.

3. Create and enter the build directory (optional but recommended):  
   ```sh
   mkdir build
   cd build
   ```
   > Creating a dedicated build directory simplifies the management of files generated during the build/compilation process.

4. Run `cmake` to generate build files:  
   ```sh
   cmake ..
   ```
   > The path following `cmake` is the location of the CMakeLists.txt file.

5. Compile the library files:  
   ```sh
   cmake --build .
   ```
   > Or, if using GNU make, compile directly using the `make` command in the Makefile's directory:
   >
   > ```shell
   > make
   > ```

#### Using Precompiled Binary Files

Precompiled binary files (dynamic libraries) for Linux, Mac OS, and Windows platforms are available in the "Releases" section. Download the file suitable for your platform, extract it to your chosen location, and then properly link it to your project.

In this case, you will only need to include the `thread_pool.hpp` header file in your project and call the corresponding interfaces.

The method for linking dynamic libraries depends on your build system/compilation tools. Refer to the documentation/references for the tool(s) you are using.

## Usage

To use the thread pool library, include the `thread_pool.hpp` header file in your project and refer to the following example code:

```cpp
#include "thread_pool.hpp"
#include <iostream>
#include <future>

void task(int id) {
    std::cout << "Hello from thread " << id << std::endl;
}

int main() {
    thread_utils::thread_pool pool(4); // Create a thread pool with 4 threads
    for (int i = 0; i < 8; ++i) {
        pool.submit(task, i);
    }
    pool.shutdown(); // Manually terminate the thread pool
    // Alternatively, let the thread pool object automatically terminate upon leaving its scope
    return 0;
}
```

You can also consult the test programs in the [tests](tests/) directory (e.g., [`functional_test.cpp`](tests/functional_test.cpp)) to see more examples and understand how to utilize various features.


## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any questions or inquiries, please reach out to me at RenPengyu2001@outlook.com.