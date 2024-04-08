#include "thread_pool.hpp"
#include <stdexcept>
#include <iostream>

namespace thread_utils {

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
thread_pool::worker_thread::~worker_thread()
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

std::int8_t thread_pool::worker_thread::terminate()
{
    std::int8_t last_status = status.exchange(0); 
    if (last_status == 2)
    {   // 如果线程被暂停，则通过信号量唤醒线程
        pause_sem.release();
    }
    return last_status;
}

void thread_pool::worker_thread::resume()
{
    status.store(1);
    pause_sem.release();    // 唤醒线程
}

} // namespace thread_utils