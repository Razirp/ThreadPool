# C++ ThreadPool Library API Documentation

Welcome to the API documentation reference for this project.

## Main Contents

This directory contains the following main documents:

- **[thread_pool.md](thread_pool.md)**: A detailed introduction to the API of the `thread_pool` class and its usage, including the creation of the thread pool, task submission, thread pool status management, thread pool property adjustment, and information retrieval.

- **[worker_thread.md](worker_thread.md)**: A detailed description of the internal implementation details of the `thread_pool::worker_thread` class. Although this class does not provide an interface directly, understanding its working principles helps to deeply understand the operation mechanism of the thread pool.

## Quick Navigation

### `class thread_pool`

- **Constructor**: How to create an instance of the thread pool, including setting the initial number of worker threads and the maximum number of tasks.
- **Task Submission**: How to use the template method `submit` to submit callable objects (functions, lambda expressions, etc.) to the thread pool for execution and obtain asynchronous execution results.
- **ThreadPool Control**:
  - **Pause**: Pause the thread pool to prevent the execution of new tasks and pause the currently running tasks.
  - **Resume**: Resume the operation of the thread pool to continue processing tasks in the queue.
  - **Wait for All Tasks to Complete**: Block the caller until all tasks in the thread pool are completed.
  - **Graceful Shutdown**: Set the thread pool to a state where it waits for tasks to complete, and then terminate the thread pool after all submitted tasks have been executed.
  - **Immediate Shutdown**: Immediately terminate the thread pool, discarding any unprocessed tasks in the task queue.
  - **Terminate**: Similar to immediate shutdown, but may use different internal implementations.
- **ThreadPool Property Adjustment**:
  - **Add Worker Threads**: Dynamically add worker threads to the thread pool.
  - **Remove Worker Threads**: Remove a specified number of worker threads from the thread pool.
  - **Set Maximum Number of Tasks**: Limit the maximum number of tasks allowed in the task queue.
- **Information Retrieval**:
  - **Get Number of Worker Threads**: Query the current number of active worker threads in the thread pool.
  - **Get Number of Tasks in Task Queue**: Query the number of tasks waiting to be executed.

### `class thread_pool::worker_thread`

- **Note**: The `thread_pool::worker_thread` class is an internal implementation detail of the thread pool and does not provide an interface directly. Its member functions and properties are called and managed internally by the `thread_pool` class.
- **Constructor**: Initialize a worker thread instance, associate it with the given thread pool, and start the thread to execute the task loop.
- **Thread State Management**: The internal implementation of operations such as termination, pause, and resume.
- **Thread Behavior**: Describes the core logic of the worker thread, such as the task loop and responding to thread pool instructions.
- **Private Data Members**: Introduces internal data structures such as the thread pool pointer, thread state, thread synchronization primitives, and the worker thread instance.

## Usage Guide

- **Dependencies**: Ensure your project correctly includes the header files required by this library and links the necessary libraries.
- **Compilation**: Follow the C++ compilation standards to ensure the thread pool library source code is compiled into your project.
- **Integration**: Introduce the `thread_utils` namespace into your code, create a `thread_pool` instance, and use the relevant functionalities as described in the above API documentation.

**Important Note**: When using this library, please follow the error handling recommendations in the documentation and ensure thread safety, especially when operating shared data in a multi-threaded environment.

For further understanding or any questions, please refer to the detailed API documentation or participate in project discussions. Thank you for your attention and use of this C++ thread pool library!
