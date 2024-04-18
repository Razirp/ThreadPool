# `thread_pool` Class API Reference

## Include Header File
```cpp
#include "thread_pool.hpp"
```

## Namespace
```cpp
namespace thread_utils
```

## Class Definition
```cpp
class thread_pool
```

### Constructor
```cpp
thread_pool(std::size_t initial_thread_count, std::size_t max_task_count = 0);
```
Parameters:
- `initial_thread_count`: The number of worker threads to create when the thread pool is initialized.
- `max_task_count`: The maximum number of tasks allowed in the task queue, with a value of 0 indicating no limit. Default value = 0.

### Destructor
```cpp
~thread_pool();
```
Automatically terminates the thread pool.

### Submit a Task
```cpp
template <typename F, typename... Args>
auto submit(F && f, Args &&... args) -> std::future<decltype(f(args...))>;
```
Parameters:
- `F`: The type of the callable object (e.g., function, function pointer, lambda expression, etc.).
- `args`: The arguments for the callable object.

Return Value:
- Returns a `std::future` object, an asynchronous accessor representing the result of the task execution.

Exceptions:
- Throws `std::runtime_error` if the thread pool is in an invalid state or the task queue is full.

### Thread Pool Control Methods

#### Pause the Thread Pool
```cpp
void pause();
```
Sets the thread pool state to paused and pauses all worker threads. If the thread pool is already in a terminated, terminating, paused, or waiting for tasks to complete state, it performs no operation.

Exceptions:
- Throws `std::runtime_error` if the thread pool state is unknown.

#### Resume the Thread Pool
```cpp
void resume();
```
Sets the thread pool state to running and resumes all worker threads. If the thread pool is already in a terminated, terminating, or running state, it performs no operation. If the thread pool is in a paused state, it resumes its operation.

Exceptions:
- Throws `std::runtime_error` if the thread pool state is unknown.

#### Wait for All Tasks to Complete
```cpp
void wait();
```
Waits for all tasks in the thread pool to complete execution. If the thread pool has already terminated, it performs no operation.

#### Graceful Shutdown of the Thread Pool
```cpp
void shutdown();
```
Sets the thread pool to a state where it waits for tasks to complete, then terminates the thread pool after all submitted tasks have finished execution. If the thread pool is already in a terminated, terminating, or waiting for tasks to complete state, it performs no operation. If the thread pool is in a paused state, it resumes its operation first.

#### Immediate Shutdown of the Thread Pool
```cpp
void shutdown_now();
```
Immediately terminates the thread pool and discards any unprocessed tasks in the task queue.

#### Terminate the Thread Pool
```cpp
void terminate();
```
Immediately terminates the thread pool, similar to `shutdown_now()`.

### Thread Pool Property Adjustment

#### Add Worker Threads
```cpp
void add_thread(std::size_t count_to_add);
```
Adds a specified number of worker threads to the thread pool. If the thread pool is in a terminated, terminating, paused, or waiting for tasks to complete state, it does not allow adding threads.

Exceptions:
- Throws `std::runtime_error` if the thread pool state is unknown.

#### Remove Worker Threads
```cpp
void remove_thread(std::size_t count_to_remove);
```
Removes a specified number of worker threads from the thread pool. If the thread pool is in a terminated, terminating, paused, or waiting for tasks to complete state, it does not allow removing threads.

Exceptions:
- Throws `std::runtime_error` if the thread pool state is unknown.

#### Set Maximum Task Count
```cpp
void set_max_task_count(std::size_t count_to_set);
```
Sets the maximum number of tasks allowed in the task queue. If the new maximum task count is less than the current number of tasks, subsequent tasks submitted will be rejected until the number of tasks falls below the newly set limit.

### Retrieve Information

#### Get the Number of Worker Threads
```cpp
std::size_t get_thread_count();
```
Returns the current number of worker threads in the thread pool.

#### Get the Number of Tasks in the Task Queue
```cpp
std::size_t get_task_count();
```
Returns the number of tasks waiting to be executed in the task queue.
