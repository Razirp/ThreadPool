C++ Thread Pool
==============

📖 **[[中文版本](README.md)]**

This is a *Thread Pool Library* implemented using modern C++ Standard Library, offering excellent cross-platform compatibility across platforms supporting C++20 or later features, including Linux, macOS, and Windows.

## Introduction

In modern software development, multi-threaded programming has become a crucial means for enhancing application performance and achieving concurrent task handling. However, directly managing multiple threads often introduces increased complexity, resource waste, and synchronization issues. To address these challenges, thread pools emerged as an effective thread management mechanism, pre-creating a set of worker threads to which tasks are submitted. The pool's threads then assume responsibility for task distribution and execution, simplifying thread management, reducing system overhead, and improving resource utilization.

This project presents a simple yet powerful thread pool implementation built upon the modern C++ Standard Library, accompanied by comprehensive documentation and characterized by usability, extensibility, configurability, and cross-platform compatibility.

### Key Benefits

- **Usability**: Streamlines the process of incorporating multithreading into applications, shortening development time and reducing complexity.
- **Efficient Resource Allocation**: Automates thread lifecycle management and assigns tasks to available threads, minimizing the overhead of frequent thread instantiation and destruction while maximizing CPU utilization.
- **Scalability**: Supports dynamic resizing of the thread pool in response to workload fluctuations, ensuring adaptability to varying resource utilization requirements.
- **Configurability**: Offers fine-grained control over thread pool behavior, such as adjusting thread count, setting task queue limits, and controlling pool suspension, resumption, and termination.
- **Cross-Platform Compatibility**: Built upon modern C++ Standard Library features, ensuring compatibility across platforms implementing C++20 or later Standard Library features, including Linux, macOS, and Windows, etc.

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

### Building from Source Code

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

#### Known Issues

When building this thread pool library using the Microsoft Visual C++ (`cl`) compiler on the Windows platform, you may encounter the following linking error:

```shell
LINK : fatal error LNK1104: cannot open file 'Debug\ThreadPool.lib' [xxx\ThreadPool\build\FunctionalTest.vcxproj]
```

While the root cause of this issue remains unclear at present, it has been observed that employing the `g++` compiler from the MinGW-w64 toolchain does not trigger this problem. Given this circumstance, we presently recommend Windows users opt for `g++` compilation to ensure a smooth build process. 

> We sincerely welcome insights and potential solutions from experts or community members who have knowledge of the underlying cause of this issue!

### Using Precompiled Binary Files

Precompiled binary files (dynamic libraries) for Linux, macOS, and Windows platforms are available in the "Releases" section. Download the file suitable for your platform, extract it to your chosen location, and then properly link it to your project.

In this case, you will only need to include the `thread_pool.hpp` header file in your project and call the corresponding interfaces.

The method for linking dynamic libraries depends on your build system/compilation tools. Refer to the documentation/references for the tool(s) you are using.

## Documentation

### API Reference

For API reference of this thread pool library, please consult the [directory](docs/API%20docs/).

### Technical Blog

[This blog](docs/blogs/blog.md) provides an introduction to the specific framework and implementation details of this thread pool library.

## Usage

To utilize the thread pool library, include the `thread_pool.hpp` header file in your project and invoke the corresponding thread pool library APIs.

Here is a basic example showcasing how to create a thread pool, submit various types of tasks, and control its state using this thread pool library.

```cpp
#include "thread_pool.hpp"

// Define a simple computational task
double compute(int x, int y) {
    return static_cast<double>(x) / y;
}

int main() {
    // Create a thread pool with an initial 4 worker threads and a task queue capacity of 100
    thread_utils::thread_pool pool(4, 100);

    // Submit a computational task, obtaining a future for the result
    auto future = pool.submit(compute, 100, 5);

    // Submit a lambda task
    pool.submit([]() {
        std::cout << "Hello from a lambda task!" << std::endl;
    });

    // Pause the thread pool, preventing execution of new tasks
    pool.pause();

    // ... Perform other operations during this period ...

    // Resume the thread pool, resuming task processing
    pool.resume();

    // Wait for the computational task to complete and retrieve the result
    double result = future.get();
    std::cout << "Result: " << result << std::endl;

    // Shut down the thread pool, waiting for all tasks to finish
    pool.shutdown();

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

---

Given the author's personal limitations in ability and time, there may inevitably be oversights or errors present within both the documentation and code of this project. Should you come across any such inaccuracies in either the documentation or code, please feel free to bring them to the author's attention via issues, pull requests, or email. Your valuable feedback will greatly contribute to the ongoing improvement of this project!