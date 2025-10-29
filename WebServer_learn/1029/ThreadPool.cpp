#include <string.h>
#include <iostream>

#include "./ThreadPool.h"
#include "TaskQueueQ.h"

ThreadPool::ThreadPool() : minnum(2), maxnum(8), busynum(0), destorynum(0), isstop(false)
{
    // 这里可以使用智能指针来解决new 失败的情况，会自动释放
    try
    {
        TaskQue = new TaskQueue;
        // 创建一个大小为maxnum的数组，存放线程ID
        pthreadID = new pthread_t[maxnum];
    }
    catch (...)
    {
        if (TaskQue)
            delete TaskQue;
        if (pthreadID)
            delete[] pthreadID;
        std::cout << "创建失败" << std::endl;
        return;
    };
    memset(pthreadID, 0, sizeof(pthreadID));

    // this 作为函数参数传入，变为了void*类型，所以要在函数中将其转变为ThreadPool*
    pthread_create(&managerID, NULL, managerfun, this);

    for (int i = 0; i < minnum; i++)
    {
        pthread_create(&pthreadID[i], NULL, workerfun, this);
    }
    alivenum = minnum;
}

ThreadPool::ThreadPool(int min, int max) : minnum(min), maxnum(max), busynum(0), destorynum(0), isstop(false)
{
    // 这里可以使用智能指针来解决new 失败的情况，会自动释放
    try
    {
        TaskQue = new TaskQueue;
        // 创建一个大小为maxnum的数组，存放线程ID
        pthreadID = new pthread_t[maxnum];
    }
    catch (...)
    {
        if (TaskQue)
            delete TaskQue;
        if (pthreadID)
            delete[] pthreadID;
        std::cout << "创建失败" << std::endl;
        return;
    };
    memset(pthreadID, 0, sizeof(pthreadID));

    // this 作为函数参数传入，变为了void*类型，所以要在函数中将其转变为ThreadPool*
    pthread_create(&managerID, NULL, managerfun, this);

    for (int i = 0; i < minnum; i++)
    {
        pthread_create(&pthreadID[i], NULL, workerfun, this);
    }
    alivenum = minnum;
}

ThreadPool::~ThreadPool()
{
    // 线程池关闭
    isstop = true;
    delete TaskQue;
    delete[] pthreadID;

    cv.notify_all();
    // 将管理者线程加入join
    // 工作线程再被唤醒后，会执行函数中对应的自杀部分
    pthread_join(managerID, NULL);
}

// 类外声明时不能重复写static
void *ThreadPool::managerfun(void *args)
{
    ThreadPool *pool = static_cast<ThreadPool *>(args);

    // 存在空闲任务
    if (pool->TaskQue->getsize() > 0 && pool->alivenum < pool->maxnum)
    {
    }
}