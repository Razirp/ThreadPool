C++ Thread Pool
==============

📖 **[[中文版本](README.md)]**

This is an implementation of a C++ thread pool based on modern C++, designed to efficiently execute concurrent tasks using a thread pool, thereby avoiding the overhead of frequently creating and destroying threads.

## Introduction

The **ThreadPool** Library is a robust and highly-configurable C++ library designed to simplify concurrent programming by providing an efficient mechanism for managing a pool of worker threads. It abstracts away the intricacies of thread creation, synchronization, and task scheduling, allowing developers to focus on implementing their application logic while enjoying improved performance and resource utilization.

### Key Benefits

- **Ease of use**: Simplifies the process of incorporating multithreading into applications, reducing development time and complexity.
- **Efficient resource allocation**: Automatically manages thread lifecycles and distributes tasks among available threads, minimizing overhead and maximizing CPU utilization.
- **Scalability**：Enables dynamic resizing of the thread pool in response to workload fluctuations, ensuring adaptability to varying resource utilization requirements.
- **Configurability**: Offers fine-grained control over thread pool behavior, such as adjusting thread count, setting task queue limits, and controlling pool suspension and termination.

## Features

### Thread Management

- **Dynamic thread pool**: Maintains a pool of worker threads responsible for executing tasks submitted by the application. The pool size can be manually adjusted at runtime to accommodate varying workloads.

### Task Scheduling and Control

- **Task queue**: Accepts tasks for execution and organizes them according to the chosen scheduling policy. The queue length can be limited, and when exceeded, new submissions will be rejected until space becomes available.
- **Scheduling Policies**: Currently supported is the First-In, First-Out (FIFO) strategy, with plans to introduce additional scheduling policies in the future.

### Pool Suspension and Resumption

- **Pause and resume**: Allows the thread pool to be temporarily suspended, causing all active threads to block and wait for further instructions. Resume operation unblocks the threads and resumes normal task processing.

### Termination Modes

- **Graceful shutdown**: Provides the option to wait for all currently queued tasks to complete before terminating the thread pool. This ensures a clean exit without losing any work in progress.
- **Immediate termination**: Optionally, the thread pool can be terminated immediately, discarding any remaining tasks in the queue. This behavior is triggered by the natural destruction of the thread pool instance, ensuring resources are released promptly.
  - This termination mode is triggered upon the natural destruction of the thread pool instance, ensuring prompt and proper resource release.


## Target Audience

The ThreadPool Library is ideal for:

- **C++ developers** seeking to incorporate multithreading into their applications without the need for low-level thread management.
- **Performance-sensitive applications** such as server backends, real-time data processing, and computationally intensive tasks that benefit from parallel execution.
- **Open-source projects** looking to enhance their concurrency capabilities by integrating a reliable and well-documented thread pool component.
- **Learners**：Individuals interested in deepening their understanding of C++, multi-threaded programming, and thread pool concepts can gain valuable practical insights by studying this library's source code, documentation, and examples.

## Project File Structure

```plaintext
ThreadPool/	
├── README.md			# Chinese README
├── README_EN.md		# English README
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

## TODO List

### Planned Features

- **Additional Task Scheduling Policies**: Currently only supports First-In, First-Out (FIFO) strategy; upcoming enhancements will introduce other algorithms like priority scheduling and timer-based scheduling to cater to diverse use cases.

- **Core Thread Count and Maximum Thread Count Concepts**: Introduce the notions of core thread count and maximum thread count, enabling the thread pool to automatically adjust thread numbers based on workload fluctuations. Core threads remain in the pool consistently, while the maximum thread count sets an upper bound for dynamic expansion.

- **Enhanced Task Rejection Policies**: Implement an array of richer rejection policies, including user-definable ones, to handle task queue saturation scenarios more flexibly.

- **Thorough Testing and Validation**: Conduct comprehensive functional testing, performance benchmarking, and comparative analysis against existing thread pool implementations to demonstrate the robustness and efficiency of this library.

## Call for Contributions

### Open to the Community

We warmly invite members of the open-source community to contribute in the following areas:

- **Feature Implementation**: Assist in completing planned features listed in the TODO section, such as implementing new scheduling strategies, supporting core and maximum thread counts, or customizing task rejection policies.

- **Testing and Benchmarks**: Contribute additional functional test cases, performance data, and especially comparative results against other thread pool libraries, enhancing the testing coverage and performance evaluation of this library.

- **Practical Experience Sharing**: Solicit community members' experiences in configuring, optimizing, troubleshooting, and securing this thread pool library, providing valuable guidance for fellow users.


Developers interested in contributing are encouraged to submit Pull Requests or raise Issues via GitHub, collaborating in the advancement of this thread pool library.


## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any questions or inquiries, please reach out to me at RenPengyu2001@outlook.com.