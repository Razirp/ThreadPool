#include "ThreadPool.hpp"

namespace thread_utils
{

std::thread&& thread_pool::worker_thread::make_thread()
{   // 创建线程，返回线程对象的右值引用
    return std::thread(
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
    );
}

thread_pool::thread_pool(
    size_t initial_thread_count, 
    size_t max_thread_count, 
    size_t max_task_count = 0
) : max_thread_count((initial_thread_count > max_thread_count) ? initial_thread_count : max_thread_count), 
    max_task_count(max_task_count),
    status(1)
{   // 同时指定初始线程数量和最大线程数量，但当初始线程数量大于最大线程数量时，最大线程数量被设置为初始线程数量
    for (size_t i = 0; i < initial_thread_count; i++)
    {   // 创建线程
        worker_list.push_back(worker_thread(this));
    }
}

thread_pool::~thread_pool()
{
    terminate();
}

void thread_pool::terminate()
{
    status.store(0);    // 设置线程池的状态为终止
    for (auto& worker : worker_list)
    {   // 终止所有线程
        worker.terminate();
    }
    task_queue_cv.notify_all(); // 唤醒所有等待任务的阻塞线程
}

void thread_pool::pause()
{
    status.store(2);    // 设置线程池的状态为暂停
    for (auto& worker : worker_list)
    {   // 暂停所有线程
        worker.pause();
    }
}

void thread_pool::resume()
{
    status.store(1);    // 设置线程池的状态为运行
    for (auto& worker : worker_list)
    {   // 恢复所有线程
        worker.resume();
    }
}

template<typename F, typename... Args>
auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{   // 提交任务
    using return_type = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::shared_mutex> lock(task_queue_mutex);
        if (max_task_count > 0 && task_queue.size() >= max_task_count)
        {   // 如果任务队列已满，则等待条件变量唤醒
            task_queue_cv.wait(lock, [this](){
                return task_queue.size() < max_task_count;
            });
        }
        task_queue.emplace([task](){ (*task)(); });
    }
    task_queue_cv.notify_one();
    return res;
}



}


