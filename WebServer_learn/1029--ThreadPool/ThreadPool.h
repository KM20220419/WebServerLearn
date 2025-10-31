#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include "TaskQueue.h"

class TaskQueue;
class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(int min, int max);
    ~ThreadPool();
    // 这里使用静态成员函数，确保可以使用pthread_create
    static void *managerfun(void *args);
    static void *workerfun(void *args);
    void addtask_thread(std::function<void()> task);
    void killthread();

private:
    int minnum;     // 最小的线程数
    int maxnum;     // 最大的线程数
    int alivenum;   // 存在的线程数
    int busynum;    // 运行的线程数
    int destorynum; // 销毁的线程数

    std::mutex mtx;
    std::condition_variable cv;
    // 存放管理者线程ID
    pthread_t managerID;
    // 存放工作线程的ID数组
    pthread_t *pthreadID;

    TaskQueue *TaskQue; // 任务队列

    bool isstop; // 是否关闭线程池
};