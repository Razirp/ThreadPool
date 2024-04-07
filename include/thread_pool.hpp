#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

// Include any necessary headers here
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <cstdint>
#include <semaphore>
#include <shared_mutex>
#include <future>
#include <iostream>
#include <stdexcept>

// Declare your class or functions here

/**
 * @namespace thread_utils
 * @brief A namespace that contains classes and functions related to thread utilities.
 */
namespace thread_utils
{

/**
 * @class thread_pool
 * @brief Represents a thread pool for executing tasks concurrently.
 * 
 * The thread_pool class provides a mechanism for executing tasks concurrently using a pool of worker threads.
 * It manages a queue of tasks and assigns them to worker threads for execution.
 * The thread pool can be paused, resumed, or terminated based on the status set by the user.
 * It also allows dynamically adding or removing worker threads from the pool.
 * The maximum number of tasks in the queue can be set to limit the number of pending tasks.
 */
class thread_pool {
private:
    /**
         * @class worker_thread
         * @brief Represents a worker thread in a thread pool.
         * 
         * The worker_thread class is responsible for executing tasks from the task queue of a thread pool.
         * It runs in a separate thread and continuously checks for tasks to execute.
         * The thread can be paused, resumed, or terminated based on the status set by the thread pool.
         */
    class worker_thread
    {
    private:
        std::atomic<std::int8_t> status;       // 状态变量，0: 线程将终止；1: 线程正在运行; 2: 线程被暂停；3: 线程在等待任务
        std::binary_semaphore pause_sem;       // 信号量，用于线程暂停时的阻塞
        thread_pool *pool;      // 线程池
        std::thread thread;     // 工作线程
    public:
        
        worker_thread(thread_pool* pool) : 
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

        ~worker_thread()
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

        std::int8_t get_status() const
        {
            return status.load();
        }


        std::int8_t terminate()
        {
            std::int8_t last_status = status.exchange(0); 
            if (last_status == 2)
            {   // 如果线程被暂停，则通过信号量唤醒线程
                pause_sem.release();
            }
            return last_status;
        }

        void pause()
        {
            status.store(2);
        }

        void resume()
        {
            status.store(1);
            pause_sem.release();    // 唤醒线程
        }
    };

    std::atomic<std::int8_t> status;        // 线程池的状态，0: 线程池将终止；1: 线程池正在运行；2: 线程池被暂停
    std::atomic<std::size_t> max_task_count;                  // 任务队列中任务的最大数量，0表示没有限制    // 当任务队列中的任务数量将或已经超过此值时，新提交的任务将被拒绝
    std::shared_mutex task_queue_mutex;         // 任务队列的互斥锁
    std::shared_mutex worker_list_mutex;        // 线程列表的互斥锁
    std::condition_variable_any task_queue_cv; // 任务队列的条件变量
    std::queue<std::function<void()>> task_queue;      // 任务队列，其中存储着待执行的任务
    std::list<worker_thread> worker_list;     // 线程列表，其中存储着工作线程

    
public:

    thread_pool(
        std::size_t initial_thread_count, 
        std::size_t max_task_count = 0) : 
        max_task_count(max_task_count), 
        status(1)
    {   
        for (std::size_t i = 0; i < initial_thread_count; ++i)
        {   // 创建线程并插入线程列表
            worker_list.emplace_back(this);     // 隐式调用worker_thread::worker_thread(thread_pool* pool)构造函数
        }
    }
    
    ~thread_pool();

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
    
    void pause();
    void resume();
    void terminate();
    void add_thread(std::size_t count_to_add)
    {   // 添加线程
        std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
        for (std::size_t i = 0; i < count_to_add; ++i)
        {
            worker_list.emplace_back(this);
        }
    }

    void remove_thread(std::size_t count_to_remove);
    void set_max_task_count(std::size_t count_to_set)
    {   // 设置任务队列中任务的最大数量；如果设置后的最大数量小于当前任务数量，则会拒绝新提交的任务，直到任务数量小于等于最大数量
        max_task_count.store(count_to_set);
    }

    std::size_t get_thread_count()
    {   // 获取线程池中线程的数量
        std::shared_lock<std::shared_mutex> lock(worker_list_mutex);
        return worker_list.size();
    }

    std::size_t get_task_count()
    {   // 获取任务队列中任务的数量
        std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
        return task_queue.size();
    }
};

/**
 * Submits a task to the thread pool for execution.
 *
 * This function submits a task to the thread pool for execution. The task is specified by the function `f` and its arguments `args`.
 * The return type of the task is deduced using `decltype(f(args...))`.
 *
 * @tparam F The type of the function `f`.
 * @tparam Args The types of the arguments `args`.
 * @param f The function to be executed as a task.
 * @param args The arguments to be passed to the function `f`.
 * @return A `std::future` object representing the result of the task.
 * @throws std::runtime_error If the thread pool is not in a valid state to accept new tasks or if the task queue is full.
 */
template<typename F, typename... Args>
auto thread_pool::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{   // 提交任务
    // 先检查是否可以提交任务
    if (std::int8_t current_status = this->status.load(); current_status != 1)
    {   // 如果线程池的状态不是运行状态，则拒绝提交任务
        switch(current_status)
        {
            case 0:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is terminated");
            case 2:
                throw std::runtime_error("[thread_pool::submit][error]: thread pool is paused");
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

thread_pool::~thread_pool()
{
    terminate();
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

#endif // THREADPOOL_HPP