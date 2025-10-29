#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

class Threadpool
{
public:
    Threadpool(int size_);
    ~Threadpool();
    void addtask(std::function<void()> task);

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> taskqueue;
    std::mutex taskmx;
    std::condition_variable cv;
    int size; // 最大线程数
    bool stop;
};