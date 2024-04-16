# **`thread_pool` 类 API 参考**

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
class thread_pool
```

### **构造函数**
```cpp
thread_pool(std::size_t initial_thread_count, std::size_t max_task_count = 0);
```
**参数**：
- `initial_thread_count`: 线程池初始化时创建的工作线程数量。
- `max_task_count`: 任务队列中允许的最大任务数，为0时表示无限制。默认值=0。

### **析构函数**
```cpp
~thread_pool();
```
自动终止线程池。

### **提交任务**
```cpp
template <typename F, typename... Args>
auto submit(F && f, Args &&... args) -> std::future<decltype(f(args...))>;
```
**参数**：
- `F`: 可调用对象类型（如函数、函数指针、lambda 表达式等）。
- `args`: 可调用对象的参数。

**返回值**：
- 返回一个 `std::future` 对象，代表任务执行结果的异步访问器。

**抛出异常**：
- 当线程池处于无效状态或任务队列已满时，抛出 `std::runtime_error`。

### **线程池控制方法**

#### **暂停线程池**
```cpp
void pause();
```
将线程池状态设为暂停，并暂停所有工作线程。若线程池已经处于终止、将终止、暂停或已等待任务完成状态，不进行任何操作。

**抛出异常**：
- 若线程池状态未知，抛出 `std::runtime_error`。

#### **恢复线程池**
```cpp
void resume();
```
将线程池状态设为运行，并恢复所有工作线程。若线程池已处于终止、将终止或正在运行状态，不进行任何操作。若线程池处于暂停状态，则恢复其运行。

**抛出异常**：
- 若线程池状态未知，抛出 `std::runtime_error`。

#### **等待所有任务完成**
```cpp
void wait();
```
等待线程池中所有任务执行完毕。若线程池已终止，不进行任何操作。

#### **优雅关闭线程池**
```cpp
void shutdown();
```
将线程池设为等待任务完成状态，等待所有已提交任务执行完毕后终止线程池。若线程池已处于终止、将终止或已等待任务完成状态，不进行任何操作。若线程池处于暂停状态，先恢复其运行。

#### **立即关闭线程池**
```cpp
void shutdown_now();
```
立即终止线程池，丢弃任务队列中的未处理任务。

#### **终止线程池**

```cpp
void terminate();
```
立即终止线程池，与 `shutdown_now()` 相似。

### **线程池属性调整**

#### **增加工作线程**
```cpp
void add_thread(std::size_t count_to_add);
```
向线程池添加指定数量的工作线程。若线程池处于终止、将终止、暂停或已等待任务完成状态，不允许添加线程。

**抛出异常**：
- 若线程池状态未知，抛出 `std::runtime_error`。

#### **移除工作线程**
```cpp
void remove_thread(std::size_t count_to_remove);
```
从线程池移除指定数量的工作线程。若线程池处于终止、将终止、暂停或已等待任务完成状态，不允许移除线程。

**抛出异常**：
- 若线程池状态未知，抛出 `std::runtime_error`。

#### **设置最大任务数**
```cpp
void set_max_task_count(std::size_t count_to_set);
```
设置任务队列允许的最大任务数。若设置后的最大任务数小于当前任务数，后续提交的任务会被拒绝，直至任务数低于新设定的上限。

### **获取信息**

#### **获取工作线程数量**
```cpp
std::size_t get_thread_count();
```
返回线程池中当前工作的线程数量。

#### **获取任务队列中任务数量**
```cpp
std::size_t get_task_count();
```
返回任务队列中等待执行的任务数量。
