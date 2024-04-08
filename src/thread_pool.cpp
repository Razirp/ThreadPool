#include "thread_pool.hpp"

namespace thread_utils {

/**
 * @brief Constructs a thread pool object.
 * 
 * @param initial_thread_count The initial number of threads in the thread pool.
 * @param max_task_count The maximum number of tasks that can be queued in the thread pool. 
 *                       If set to 0, there is no limit on the number of tasks.
 */
thread_pool::thread_pool(
    std::size_t initial_thread_count, 
    std::size_t max_task_count
) :
    max_task_count(max_task_count), 
    status(1)
{   
    for (std::size_t i = 0; i < initial_thread_count; ++i)
    {   // 创建线程并插入线程列表
        worker_list.emplace_back(this);     // 隐式调用worker_thread::worker_thread(thread_pool* pool)构造函数
    }
}

thread_pool::~thread_pool()
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


std::size_t thread_pool::get_thread_count()
{   // 获取线程池中线程的数量
    std::shared_lock<std::shared_mutex> lock(worker_list_mutex);
    return worker_list.size();
}

std::size_t thread_pool::get_task_count()
{   // 获取任务队列中任务的数量
    std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
    return task_queue.size();
}

} // namespace thread_utils