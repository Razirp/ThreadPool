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

// Declare your class or functions here

namespace thread_utils
{

class thread_pool {
private:
    class worker_thread
    {
    private:
        std::atomic<std::uint8_t> status;       // 状态变量，0: 线程被终止；1: 线程正在运行; 2: 线程被暂停；3: 线程在等待任务
        std::binary_semaphore pause_sem;       // 信号量，用于线程暂停时的阻塞
        thread_pool *pool;      // 线程池
        std::thread thread;     // 工作线程
    public:
        worker_thread(thread_pool* pool) : pool(pool), status(1), pause_sem(0), thread(std::move(make_thread())) {}

        ~worker_thread()
        {
            terminate();
            if (thread.joinable())
            {
                thread.join();
            }
        }

        std::thread&& make_thread();        // 创建线程，返回线程对象的右值引用

        std::uint8_t get_status()
        {
            return status.load();
        }


        void terminate()
        {
            switch (status.load())
            {
            case 0:
                break;
            case 1:
            case 3:
                status.store(0);
                break;
            case 2:
                status.store(0);
                pause_sem.release();
                break;
            default:
                break;
            }
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

    std::atomic<std::uint8_t> status;        // 线程池的状态，0: 线程池被立即终止；1: 线程池正在运行；2: 线程池被暂停；3: 线程池不再接受新的任务，在等待已有任务全部完成后终止
    size_t max_thread_count;                // 线程池中线程的最大数量，0表示没有限制
    size_t max_task_count;                  // 任务队列中任务的最大数量，0表示没有限制
    std::shared_mutex task_queue_mutex;         // 任务队列的互斥锁
    std::shared_mutex worker_list_mutex;        // 线程列表的互斥锁
    std::condition_variable_any task_queue_cv; // 任务队列的条件变量
    std::queue<std::function<void()>> task_queue;      // 任务队列，其中存储着待执行的任务
    std::list<worker_thread> worker_list;     // 线程列表，其中存储着工作线程

    
    
public:
    thread_pool(size_t initial_thread_count, size_t max_task_count = 0)   
    {   // 仅指定初始线程数量，此时最大线程数量与初始线程数量相同
        thread_pool(initial_thread_count, initial_thread_count, max_task_count);
    }

    thread_pool(size_t initial_thread_count, size_t max_thread_count, size_t max_task_count = 0);
    
    ~thread_pool();
    template<typename F, typename... Args>
    auto commit(F&& f, Args&&... args);
    void pause();
    void resume();
    void terminate();
    void remove_thread(size_t count);
    void set_max_thread_count(size_t count);
    void set_max_task_count(size_t count);

    size_t get_thread_count()
    {   // 获取线程池中线程的数量
        std::shared_lock<std::shared_mutex> lock(worker_list_mutex);
        return worker_list.size();
    }

    size_t get_task_count()
    {   // 获取任务队列中任务的数量
        std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
        return task_queue.size();
    }
};

}

#endif // THREADPOOL_HPP