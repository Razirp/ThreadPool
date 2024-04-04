#include <functional>
// #include <pthread.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <cstdint>
#include <semaphore>

namespace thread_utils
{
class task_entry
{
    // 队列：双向链表/环形链表
    // task_entry *next;
private:
    void *task_data;
    std::function<void(void*)> task_callback;
public:
    task_entry(std::function<void(void*)> task_callback, void *task_data): task_callback(task_callback), task_data(task_data) {}

    void run()
    {
        task_callback(task_data);
    }

};


class thread_pool {
private:
    class worker_thread
    {
    private:
        std::atomic<std::uint8_t> status;       // 状态变量，0: 线程被终止；1: 线程正在运行; 2: 线程被暂停
        std::binary_semaphore pause_sem;       // 信号量，用于线程暂停时的阻塞
        thread_pool *pool;      // 线程池
        std::thread thread;     // 工作线程
    public:
        worker_thread(thread_pool* pool) : pool(pool), status(1), pause_sem(0), thread(make_thread()) {}

        ~worker_thread()
        {
            terminate();
            if (thread.joinable())
            {
                thread.join();
            }
        }

        std::thread&& make_thread();        // 创建线程，返回线程对象的右值引用

        void terminate()
        {
            status.store(0);
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

    std::uint8_t status;        // 线程池的状态，0: 线程池被终止；1: 线程池正在运行；2: 线程池被暂停
    size_t max_thread_count;                // 线程池中线程的最大数量，0表示没有限制
    size_t max_task_count;                  // 任务队列中任务的最大数量，0表示没有限制
    std::mutex task_queue_mutex;         // 任务队列的互斥锁
    std::mutex worker_list_mutex;         // 工作队列的互斥锁
    std::condition_variable task_queue_cv; // 任务队列的条件变量
    std::queue<task_entry> task_queue;      // 任务队列，其中存储着待执行的任务
    std::list<worker_thread> worker_list;     // 线程列表，其中存储着工作线程

    
    
public:
    bool is_stopped;                    // 线程池是否停止
    thread_pool(size_t initial_thread_count, size_t max_task_count = 0);   // 仅指定初始线程数量，此时最大线程数量与初始线程数量相同
    thread_pool(size_t initial_thread_count, size_t max_thread_count, size_t max_task_count = 0);  // 同时指定初始线程数量和最大线程数量，但当初始线程数量大于最大线程数量时，最大线程数量被设置为初始线程数量
    ~thread_pool();
    template<typename F, typename... Args>
    void commit(F&& f, Args&&... args);
    void start();
    void stop();
};


std::thread&& thread_pool::worker_thread::make_thread()
{   // 创建线程，返回线程对象的右值引用
    return std::thread(
        [this](){       // 线程的执行逻辑
            while (this->status.load() != 0) // 在没有被设置为终止的情况下
            {
                while (this->status.load() == 2)
                {   // 如果线程被暂停，则阻塞线程，等待信号量唤醒
                    this->pause_sem.acquire();  // 阻塞线程
                }
                // 在运行状态下，从任务队列中取出一个任务并执行
                std::unique_lock<std::mutex> lock(this->pool->task_queue_mutex);    // 在取任务前，加锁
                if (this->pool->task_queue.empty())
                {   // 如果任务队列为空，则等待条件变量唤醒
                    this->pool->task_queue_cv.wait(lock, [this](){
                        return !this->pool->task_queue.empty();
                    }); // 等待条件变量唤醒； 相当于：while(this->pool->task_queue.empty()) { this->pool->task_queue_cv.wait(lock); }
                }
                // 取出一个任务
                task_entry task = this->pool->task_queue.front();
                this->pool->task_queue.pop();
                lock.unlock();  // 取出任务后，释放锁
                task.run();     // 执行任务
            }
        }
    );
}

}

int main(int argc, char *argv[])
{
    return 0;
}
