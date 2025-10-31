#include <queue>
#include <mutex>
#include <functional>

class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    void addtask(std::function<void()> taskfunc);
    std::function<void()> getTask();
    int getqueue_size();

private:
    // 定义互斥锁 --- 访问队列时互斥访问
    std::mutex mtx;
    // 定义条件变量
    std::queue<std::function<void()>> taskQueue;
};