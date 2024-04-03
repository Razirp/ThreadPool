#include <functional>
// #include <pthread.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace thread_util
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
    size_t thread_count;                // 线程池中线程的数量
    std::queue<task_entry> task_queue;      // 任务队列，其中存储着待执行的任务
    std::queue<std::thread> work_queue;     // 线程池，线程队列，其中存储着工作线程
public:
    bool is_stopped;                    // 线程池是否停止
    thread_pool(size_t initial_thread_count);
    ~thread_pool();
    void add_task(std::function<void(void*)> task_callback, void *task_data);
    void start();
    void stop();
};

thread_pool::thread_pool(size_t initial_thread_count): thread_count(initial_thread_count), is_stopped(false)
{
    for (int i = 0; i < this->thread_count; i++)
    {
        work_queue.push(std::thread([this](){
            while (!this->is_stopped)
            {
                if (!task_queue.empty())
                {
                    task_entry task = task_queue.front();
                    task_queue.pop();
                    task.run();
                }
            }
        }));
    }
}

}

int main(int argc, char *argv[])
{
    return 0;
}
