# **`thread_pool::worker_thread` 类 API 参考**

## **包含头文件**

```cpp
#include "thread_pool.hpp"
```

## **命名空间**

```cpp
namespace thread_utils
```

## **类定义**
```cpp
class thread_pool::worker_thread
```

**注意**: `thread_pool::worker_thread` 类是 `thread_pool` 类的内部实现细节，不直接对外提供接口，其成员函数和属性由 `thread_pool` 类内部调用和管理。

### **构造函数**
```cpp
worker_thread(thread_pool *pool);
```
**参数**:
- `pool`: 指向所属 `thread_pool` 实例的指针。

**功能**:
- 初始化工作线程实例，将其与给定的线程池关联，并启动线程执行任务循环。

### **析构函数**
```cpp
~worker_thread();
```
**功能**:
- 终止工作线程，并确保其安全退出。

### **线程状态管理**

#### **终止线程**
```cpp
status_t terminate();
```
**返回值**:
- 返回线程在终止前的原状态。

**功能**:

- 在锁定线程状态的情况下，将线程状态设置为将终止（TERMINATING），并返回原状态。如果线程正在运行或等待任务，将其置为将终止状态以便后续处理。

#### **暂停线程**
```cpp
void pause();
```
**功能**:
- 在锁定线程状态的情况下，将线程状态设置为暂停（PAUSED）。如果线程正在运行，将其置为暂停状态。

#### **恢复线程**
```cpp
void resume();
```
**功能**:
- 在锁定线程状态的情况下，将线程状态从暂停（PAUSED）恢复到运行（RUNNING）。如果线程处于暂停状态，将其置回运行状态。

### **私有数据成员**

#### **线程池指针**
```cpp
thread_pool *pool;
```
**描述**:
- 指向所属 `thread_pool` 实例的指针，用于访问线程池资源和接收指令。

#### **线程状态**
```cpp
std::atomic<status_t> status;
```
**描述**:
- 使用原子整型变量存储线程状态，包括运行（RUNNING）、等待任务（BLOCKED）、暂停（PAUSED）、将终止（TERMINATING）、已终止（TERMINATED）等。

#### **线程同步原语**
```cpp
std::binary_semaphore pause_sem;
std::shared_mutex status_mutex;
```
**描述**:
- 使用二进制信号量控制线程的暂停与恢复，使用共享互斥锁保护线程状态的访问。

#### **工作线程**
```cpp
std::thread thread;
```
**描述**:
- 存储工作线程实例，用于启动和管理线程的生命周期。