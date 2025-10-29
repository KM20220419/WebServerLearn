#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

class TaskQueue;
class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(int min, int max);
    ~ThreadPool();
    void addtask_pool(std::function<void()> func);
    void workerthreadfun();
    void managerthreadfun();
    // void killthread();

private:
    // 最大线程数
    int maxnum;
    // 最小线程数
    int minnum;
    // 正在工作的线程数
    int busynum;
    // 存活线程数
    int alivenum;
    // 销毁的线程数
    int destorynum;

    // 任务队列
    TaskQueue *taskque;

    // 存放工作线程
    std::vector<std::thread> workthreads;

    // 管理线程
    std::thread managerthread;

    // 管理线程池的锁
    std::mutex poolmtx;
    // 条件变量
    std::condition_variable cv;

    // 是否关闭线程池
    bool isstop;
};