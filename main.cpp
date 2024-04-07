#include "thread_pool.hpp"

int main()
{
    thread_utils::thread_pool pool(5, 100);
    std::vector<std::future<int> > results;
    int upper = 10;
    for(int i = 0; i < upper; ++i) {
        results.emplace_back(
            pool.submit([i] {
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "world " << i << std::endl;
                return i*i;
            })
        );
        if(i == 5){
            pool.add_thread(10);
            pool.remove_thread(5);
            pool.pause();
            pool.resume();
            pool.set_max_task_count(upper / 2);
        }
        

    }
    // pool.terminate();
    // std::cout << "pool terminated" << std::endl;
    for(auto && result: results)
        std::cout << "result:" << result.get() << ' ';
    std::cout << std::endl;
    
    return 0;
}