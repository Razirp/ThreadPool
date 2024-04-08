#include "thread_pool.hpp"
#include <stdexcept>
#include <iostream>

namespace thread_utils {

/**
 * @brief Constructs a thread pool object.
 * 
 * @param initial_thread_count The initial number of threads in the thread pool.
 * @param max_task_count The maximum number of tasks that can be queued in the thread pool. 
 *                       If set to 0, there is no limit on the number of tasks.
 */
inline thread_pool::thread_pool(
    std::size_t initial_thread_count, 
    std::size_t max_task_count = 0
) :
    max_task_count(max_task_count), 
    status(1)
{   
    for (std::size_t i = 0; i < initial_thread_count; ++i)
    {   // 创建线程并插入线程列表
        worker_list.emplace_back(this);     // 隐式调用worker_thread::worker_thread(thread_pool* pool)构造函数
    }
}

inline thread_pool::~thread_pool()
{
    terminate();
}

/**
 * @brief Pauses the thread pool.
 * 
 * This function sets the status of the thread pool to "paused" and pauses all the worker threads.
 * 
 * @note This function is thread-safe.
 */
void thread_pool::pause()
{
    status.store(2);    // 设置线程池的状态为暂停
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (auto& worker : worker_list)
    {   // 暂停所有线程
        worker.pause();
    }
}

/**
 * Resumes the execution of the thread pool.
 * Sets the status of the thread pool to running and resumes all worker threads.
 */
void thread_pool::resume()
{
    status.store(1);    // 设置线程池的状态为运行
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (auto& worker : worker_list)
    {   // 恢复所有线程
        worker.resume();
    }
}

/**
 * @brief Terminates the thread pool.
 * 
 * This function sets the status of the thread pool to terminate and terminates all worker threads.
 * It also notifies all blocked threads waiting for tasks in the task queue.
 */
void thread_pool::terminate()
{
    status.store(0);    // 设置线程池的状态为终止
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (auto& worker : worker_list)
    {   // 终止所有线程
        worker.terminate();
    }
    task_queue_cv.notify_all(); // 唤醒所有等待任务的阻塞线程
}

/**
 * Adds a specified number of threads to the thread pool.
 *
 * @param count_to_add The number of threads to add.
 */
void thread_pool::add_thread(std::size_t count_to_add)
{   // 添加线程
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (std::size_t i = 0; i < count_to_add; ++i)
    {
        worker_list.emplace_back(this);
    }
}

/**
 * Removes a specified number of threads from the thread pool.
 * 
 * @param count_to_remove The number of threads to remove.
 * 
 * This function removes the minimum of `count_to_remove` and the current number of threads in the thread pool.
 * It terminates the removed threads and removes them from the worker list.
 * After removing the threads, it notifies all waiting threads in the task queue to check if any threads have been terminated.
 */
void thread_pool::remove_thread(std::size_t count_to_remove)
{   // 移除min(count_to_remove, worker_list.size())个线程
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    count_to_remove = std::min(count_to_remove, worker_list.size());
    auto it = worker_list.end();
    for (std::size_t i = 0; i < count_to_remove; ++i)
    {
        --it;
        it->terminate();
    }
    task_queue_cv.notify_all(); // 唤醒所有等待任务的阻塞线程，以便它们能够检查线程是否被终止
    worker_list.erase(it, worker_list.end());   // 从线程列表中移除工作线程对象并析构
}

inline void thread_pool::set_max_task_count(std::size_t count_to_set)
{   // 设置任务队列中任务的最大数量；如果设置后的最大数量小于当前任务数量，则会拒绝新提交的任务，直到任务数量小于等于最大数量
    max_task_count.store(count_to_set);
}

inline std::size_t thread_pool::get_thread_count()
{   // 获取线程池中线程的数量
    std::shared_lock<std::shared_mutex> lock(worker_list_mutex);
    return worker_list.size();
}

inline std::size_t thread_pool::get_task_count()
{   // 获取任务队列中任务的数量
    std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
    return task_queue.size();
}

/**
 * @brief Constructs a worker thread object for the thread pool.
 * 
 * @param pool A pointer to the thread pool that owns the worker thread.
 */
thread_pool::worker_thread::worker_thread(thread_pool* pool) : 
    pool(pool), 
    status(1), 
    pause_sem(0), 
    thread(
        [this](){       // 线程的执行逻辑
            while (this->status.load() != 0) // 在没有被设置为终止的情况下
            {
                while (this->status.load() == 2)
                {   // 如果线程被暂停，则阻塞线程，等待信号量唤醒
                    this->pause_sem.acquire();  // 阻塞线程
                    if (this->status.load() == 0)
                    {   // 如果线程被设置为终止，则退出线程
                        return;
                    }
                }
                // 在运行状态下，从任务队列中取出一个任务并执行
                std::unique_lock<std::shared_mutex> lock(this->pool->task_queue_mutex);    // 在取任务前，加锁
                while (this->pool->task_queue.empty())
                {   // 如果任务队列为空，则等待条件变量唤醒
                    if (this->status.load() == 0)
                    {   // 如果线程被设置为终止，则退出线程
                        return;
                    }
                    status.store(3);    // 设置线程状态为等待任务
                    this->pool->task_queue_cv.wait(lock); // 等待条件变量唤醒；
                    if (this->status.load() == 0)
                    {   // 如果线程被设置为终止，则退出线程
                        return;
                    }
                    else
                    {
                        status.store(1);    // 设置线程状态为运行
                    }
                }
                // 取出一个任务
                try
                {
                    std::function<void()> task = std::move(this->pool->task_queue.front());
                    this->pool->task_queue.pop();
                    lock.unlock();  // 取出任务后，释放锁
                    task();     // 执行任务
                }
                catch(const std::exception& e)
                {   // 如果任务执行过程中发生异常，则打印异常信息并终止线程
                    std::cerr << e.what() << '\n';
                    this->terminate();
                    return;
                }
            }
        }
    ) {}

/**
 * @brief Destructor for the worker_thread class.
 * 
 * This destructor is responsible for terminating the worker thread and joining it if necessary.
 * If the thread was waiting for a task and has not been awakened at the time of destruction,
 * it is awakened using a condition variable to avoid blocking the destruction process.
 */
inline thread_pool::worker_thread::~worker_thread()
{
    std::int8_t last_status = terminate();
    if (thread.joinable())
    {
        if (last_status == 3)
        {   // 如果线程之前在等待任务且析构时仍未被唤醒，则通过条件变量唤醒线程，以避免析构过程被阻塞
            pool->task_queue_cv.notify_all();
        }
        thread.join();
    }
}

inline std::int8_t thread_pool::worker_thread::get_status() const
{
    return status.load();
}

std::int8_t thread_pool::worker_thread::terminate()
{
    std::int8_t last_status = status.exchange(0); 
    if (last_status == 2)
    {   // 如果线程被暂停，则通过信号量唤醒线程
        pause_sem.release();
    }
    return last_status;
}

inline void thread_pool::worker_thread::pause()
{
    status.store(2);
}

inline void thread_pool::worker_thread::resume()
{
    status.store(1);
    pause_sem.release();    // 唤醒线程
}

} // namespace thread_utils