#include <functional>
// #include <pthread.h>
#include <thread>
#include <queue>

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
    int thread_count;
    std::queue<task_entry> task_queue;
    std::queue<std::thread> work_queue;
    std::thread *threads;
    bool stop;
public:
    thread_pool(int thread_count);
    ~thread_pool();
    void add_task(std::function<void(void*)> task_callback, void *task_data);
    void start();
    void stop();
};

int main(int argc, char *argv[])
{
    return 0;
}
