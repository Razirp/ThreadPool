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

namespace thread_utils
{

class thread_pool {
private:
    class worker_thread
    {
    private:
        std::atomic<std::int8_t> status;       // 状态变量，0: 线程将终止；1: 线程正在运行; 2: 线程被暂停；3: 线程在等待任务
        std::binary_semaphore pause_sem;       // 信号量，用于线程暂停时的阻塞
        thread_pool *pool;      // 线程池
        std::thread thread;     // 工作线程
    public:
        worker_thread(thread_pool* pool) : pool(pool), status(1), pause_sem(0), thread(std::move(make_thread())) {}

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

        std::thread&& make_thread();        // 创建线程，返回线程对象的右值引用

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
            // switch (last_status)
            // {
            // case 0:
            // case 1:
            // case 3: // 考虑性能，不在此处通过条件变量notify_all唤醒线程，而是等待线程被自然唤醒时退出
            //     break;
            // case 2:
            //     pause_sem.release();
            //     break;
            // default:
            //     break;
            // }
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

    std::atomic<std::int8_t> status;        // 线程池的状态，0: 线程池将终止；1: 线程池正在运行；2: 线程池被暂停；3: 线程池不再接受新的任务，继续执行任务队列中的任务
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

}

#endif // THREADPOOL_HPP