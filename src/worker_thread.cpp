#include "thread_pool.hpp"
#include <stdexcept>
#include <iostream>

namespace thread_utils {


/**
 * @brief Constructs a worker thread object for the thread pool.
 *
 * This constructor initializes a worker thread object for the thread pool. The worker thread is responsible for executing tasks from the task queue.
 *
 * @param pool A pointer to the thread pool that owns the worker thread.
 */
thread_pool::worker_thread::worker_thread(thread_pool* pool) : 
    pool(pool), 
    status(status_t::RUNNING), 
    pause_sem(0), 
    thread(
        [this](){       // 线程的执行逻辑
            while (true)
            {   
                std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
                while(true)
                {
                    if (!unique_lock_status.owns_lock())
                    {
                        unique_lock_status.lock();
                    }
                    bool break_flag = false;
                    switch (this->status.load())
                    {
                    case status_t::TERMINATING: // 线程被设置为将终止
                        this->status.store(status_t::TERMINATED);
                    case status_t::TERMINATED:  // 线程已终止
                        return;
                    case status_t::RUNNING: // 线程被设置为运行
                        break_flag = true;
                        break;
                    case status_t::PAUSED:   // 线程被设置为暂停
                        unique_lock_status.unlock();
                        this->pause_sem.acquire();  // 阻塞线程
                        break;
                    case status_t::BLOCKED:  // 线程被设置为等待任务
                    default:    // 未知状态
                        unique_lock_status.unlock();
                        throw std::runtime_error("[thread_pool::worker_thread::worker_thread][error]: undefined status");
                    }
                    if (break_flag)
                    {
                        unique_lock_status.unlock();
                        break;
                    }
                }
                // 在运行状态下，从任务队列中取出一个任务并执行
                std::unique_lock<std::shared_mutex> unique_lock_task_queue(this->pool->task_queue_mutex);    // 在取任务前，加锁
                while (this->pool->task_queue.empty())
                {   // 如果任务队列为空，则等待条件变量唤醒
                    while(true)
                    {
                        if (!unique_lock_status.owns_lock())
                        {
                            unique_lock_status.lock();
                        }
                        bool break_flag = false;
                        switch (this->status.load())
                        {
                        case status_t::TERMINATING: // 线程被设置为将终止
                            this->status.store(status_t::TERMINATED);
                        case status_t::TERMINATED:  // 线程已终止
                            return;
                        case status_t::PAUSED:  // 线程被设置为暂停
                            unique_lock_status.unlock();
                            unique_lock_task_queue.unlock();
                            this->pause_sem.acquire();  // 阻塞线程
                            unique_lock_task_queue.lock();
                            break;
                        case status_t::RUNNING: // 线程被设置为运行
                            this->status.store(status_t::BLOCKED);  // 设置线程状态为等待任务
                        case status_t::BLOCKED:  // 线程被设置为等待任务
                            break_flag = true;
                            break;
                        default:    // 未知状态
                            unique_lock_status.unlock();
                            unique_lock_task_queue.unlock();
                            throw std::runtime_error("[thread_pool::worker_thread::worker_thread][error]: unknown status");
                        }
                        if (break_flag)
                        {
                            unique_lock_status.unlock();
                            break;
                        }
                    }
                    this->pool->task_queue_cv.wait(unique_lock_task_queue); // 等待条件变量唤醒；
                    while(true)
                    {
                        if (!unique_lock_status.owns_lock())
                        {
                            unique_lock_status.lock();
                        }
                        bool break_flag = false;
                        switch (this->status.load())
                        {
                        case status_t::TERMINATING: // 线程被设置为将终止
                            this->status.store(status_t::TERMINATED);
                        case status_t::TERMINATED:  // 线程已终止
                            return;
                        case status_t::PAUSED:  // 线程被设置为暂停
                            unique_lock_status.unlock();
                            unique_lock_task_queue.unlock();
                            this->pause_sem.acquire();  // 阻塞线程
                            unique_lock_task_queue.lock();
                            break;
                        case status_t::BLOCKED:  // 线程被设置为等待任务
                            this->status.store(status_t::RUNNING);  // 设置线程状态为运行
                        case status_t::RUNNING: // 线程被设置为运行
                            break_flag = true;
                            break;
                        default:    // 未知状态
                            unique_lock_status.unlock();
                            throw std::runtime_error("[thread_pool::worker_thread::worker_thread][error]: unknown status");
                        }
                        if (break_flag)
                        {
                            unique_lock_status.unlock();
                            break;
                        }
                    }
                }
                // 取出一个任务
                try
                {
                    std::function<void()> task = std::move(this->pool->task_queue.front());
                    this->pool->task_queue.pop();
                    if (this->pool->task_queue.empty())
                    {   // 如果任务队列为空，则通知任务队列空条件变量
                        this->pool->task_queue_empty_cv.notify_all();
                    }
                    unique_lock_task_queue.unlock();  // 取出任务后，释放锁
                    task();     // 执行任务
                }
                catch(const std::exception& e)
                {   // 如果任务执行过程中发生异常，则打印异常信息并继续循环
                    std::cerr << e.what() << '\n';
                    continue;
                }
            }
        }
    ) {}


/**
 * @brief Destructor for the worker_thread class.
 * 
 * This destructor is responsible for terminating the worker thread and joining it if necessary.
 * If the thread was previously blocked waiting for a task and is still blocked at the time of destruction,
 * it will be woken up using a condition variable to avoid blocking the destruction process.
 */
thread_pool::worker_thread::~worker_thread()
{
    status_t last_status = terminate();
    if (thread.joinable())
    {
        if (last_status == status_t::BLOCKED)
        {   // 如果线程之前在等待任务且析构时仍未被唤醒，则通过条件变量唤醒线程，以避免析构过程被阻塞
            pool->task_queue_cv.notify_all();
        }
        thread.join();
    }
}

/**
 * @brief Terminates the worker thread.
 * 
 * This function is used to terminate the worker thread. It sets the status of the worker thread to TERMINATING and returns the previous status.
 * If the worker thread is currently running, it will be paused before termination.
 * 
 * @return The previous status of the worker thread.
 */
thread_pool::worker_thread::status_t thread_pool::worker_thread::terminate_with_status_lock()
{
    status_t last_status = this->status.load();
    switch (last_status)
    {
    case status_t::TERMINATED:  // 线程已终止
    case status_t::TERMINATING: // 线程将终止
        break;
    case status_t::RUNNING: // 线程正在运行
    case status_t::BLOCKED:  // 线程在等待任务
        status.store(status_t::TERMINATING);
        break;
    case status_t::PAUSED:   // 线程被暂停
        status.store(status_t::TERMINATING);
        pause_sem.release();
        break;
    default:
        throw std::runtime_error("[thread_pool::worker_thread::terminate][error]: unknown status");
    }
    return last_status;
}

thread_pool::worker_thread::status_t thread_pool::worker_thread::terminate()
{
    std::unique_lock<std::shared_mutex> lock(status_mutex);
    return terminate_with_status_lock();
}

/**
 * Pauses the worker thread by changing its status to PAUSED.
 * If the thread's status is already TERMINATED, TERMINATING, or PAUSED, the function returns immediately.
 * If the thread's status is BLOCKED or RUNNING, the function changes the status to PAUSED.
 * If the thread's status is unknown, the function throws a std::runtime_error.
 */
void thread_pool::worker_thread::pause_with_status_lock()
{
    switch (status.load())
    {
    case status_t::TERMINATED:  // 线程已终止
    case status_t::TERMINATING: // 线程将终止
    case status_t::PAUSED:   // 线程被暂停
        return;
    case status_t::BLOCKED:  // 线程在等待任务
    case status_t::RUNNING: // 线程正在运行
        status.store(status_t::PAUSED);
        break;    
    default:
        throw std::runtime_error("[thread_pool::worker_thread::pause][error]: unknown status");
    }
}

void thread_pool::worker_thread::pause()
{
    std::unique_lock<std::shared_mutex> lock(status_mutex);
    pause_with_status_lock();
}

/**
 * Resumes the worker thread if it is currently paused.
 * If the thread is not paused, blocked, running, or terminating, an exception is thrown.
 */
void thread_pool::worker_thread::resume_with_status_lock()
{
    switch (status.load())
    {
    case status_t::TERMINATED:  // 线程已终止
    case status_t::TERMINATING: // 线程将终止
    case status_t::RUNNING: // 线程正在运行
    case status_t::BLOCKED:  // 线程在等待任务
        return;
    case status_t::PAUSED:   // 线程被暂停
        status.store(status_t::RUNNING);
        pause_sem.release();    // 唤醒线程
        break;
    default:
        throw std::runtime_error("[thread_pool::worker_thread::resume][error]: unknown status");
    }
}

void thread_pool::worker_thread::resume()
{
    std::unique_lock<std::shared_mutex> lock(status_mutex);
    resume_with_status_lock();
}

} // namespace thread_utils