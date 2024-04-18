# `thread_pool::worker_thread` Class API Reference

## Include Header
```cpp
#include "thread_pool.hpp"
```

## Namespace
```cpp
namespace thread_utils
```

## Class Definition
```cpp
class thread_pool::worker_thread
```

**Note:** The `thread_pool::worker_thread` class is an internal implementation detail of the `thread_pool` class and does not provide a direct interface. Its member functions and attributes are called and managed internally by the `thread_pool` class.

### Constructor
```cpp
worker_thread(thread_pool *pool);
```
- **Parameters:**
  - `pool`: A pointer to the `thread_pool` instance that the worker thread belongs to.
- **Functionality:**
  - Initializes a worker thread instance, associates it with the given thread pool, and starts the thread to execute a task loop.

### Destructor
```cpp
~worker_thread();
```
- **Functionality:**
  - Terminates the worker thread and ensures it exits safely.

### Thread State Management

#### Terminate Thread
```cpp
status_t terminate();
```
- **Return Value:**
  - Returns the original state of the thread before termination.
- **Functionality:**
  - Sets the thread state to terminating (TERMINATING) while locking the thread state and returns the original state. If the thread is running or waiting for a task, it is set to the terminating state for subsequent processing.

#### Pause Thread
```cpp
void pause();
```
- **Functionality:**
  - Sets the thread state to paused (PAUSED) while locking the thread state. If the thread is currently running, it is set to the paused state.

#### Resume Thread
```cpp
void resume();
```
- **Functionality:**
  - Recovers the thread state from paused (PAUSED) back to running (RUNNING) while locking the thread state. If the thread is in a paused state, it is set back to the running state.

### Private Data Members

#### Thread Pool Pointer
```cpp
thread_pool *pool;
```
- **Description:**
  - A pointer to the `thread_pool` instance that the worker thread belongs to, used for accessing thread pool resources and receiving instructions.

#### Thread Status
```cpp
std::atomic<status_t> status;
```
- **Description:**
  - An atomic integer variable that stores the thread state, including RUNNING, BLOCKED (waiting for a task), PAUSED, TERMINATING, and TERMINATED.

#### Thread Synchronization Primitives
```cpp
std::binary_semaphore pause_sem;
std::shared_mutex status_mutex;
```
- **Description:**
  - A binary semaphore is used to control the pause and resume of the thread, and a shared mutex is used to protect access to the thread state.

#### Worker Thread
```cpp
std::thread thread;
```
- **Description:**
  - Stores the instance of the worker thread, used for starting and managing the thread's lifecycle.