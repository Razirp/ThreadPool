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

thread_pool::~thread_pool()
{
    terminate();
}

template<typename F, typename... Args>
auto thread_pool::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{   // 提交任务
    // 先检查是否可以提交任务
    if (std::int8_t current_status = this->status.load(); current_status != 1)
    {   // 如果线程池的状态不是运行状态，则拒绝提交任务
        switch(current_status)
        {
            case 0:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is terminating");
            case 2:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is paused");
            case 3:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is not accepting new tasks");
            default:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is not running");
        }
    }
    else if (max_task_count > 0 && get_task_count() >= max_task_count)
    {   // 如果任务队列已满，则拒绝提交任务
        throw std::runtime_error("[thread_pool::submit][error]: task queue is full");
    }

    using return_type = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();

    std::unique_lock<std::shared_mutex> lock(task_queue_mutex);
    task_queue.emplace([task](){ (*task)(); }); // 将任务封装为一个lambda表达式并放入任务队列   // 该lambda表达式会调用std::packaged_task对象的operator()方法，从而执行任务
    lock.unlock();
    task_queue_cv.notify_one();
    return res;
}

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

void thread_pool::pause()
{
    status.store(2);    // 设置线程池的状态为暂停
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (auto& worker : worker_list)
    {   // 暂停所有线程
        worker.pause();
    }
}

void thread_pool::resume()
{
    status.store(1);    // 设置线程池的状态为运行
    std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
    for (auto& worker : worker_list)
    {   // 恢复所有线程
        worker.resume();
    }
}

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

} // namespace thread_utils
