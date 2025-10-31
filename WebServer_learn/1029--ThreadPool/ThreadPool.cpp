#include <string.h>
#include <iostream>
#include <unistd.h>

#include "ThreadPool.h"
#include "TaskQueue.h"

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
    while (!pool->isstop)
    {
        std::cout << "Thread num:" << pool->alivenum << std::endl;
        sleep(3);
        // 加锁操纵共享变量
        std::unique_lock<std::mutex> lock(pool->mtx);
        // 存在空闲任务
        if (pool->TaskQue->getsize() > 0 && pool->alivenum < pool->maxnum)
        {
            int addnum = std::min(2, pool->maxnum - pool->alivenum);
            int add = 0;
            // 在增加线程的时候，要查看之前的位置中是否存在线程ID为0的情况（可能以前被销毁的）
            for (int i = 0; i < pool->maxnum && add < addnum; i++)
            {
                if (pool->pthreadID[i] == 0)
                {
                    std::cout << "thread add " << std::endl;
                    pthread_create(&pool->pthreadID[pool->alivenum], NULL, workerfun, pool);
                    pool->alivenum++;
                    add++;
                }
            }
        }
        // 空闲线程太多，销毁
        if (pool->busynum * 2 < pool->alivenum && pool->alivenum > pool->minnum)
        {
            // std::cout << "thread jian jian" << std::endl;
            pool->destorynum = std::min(2, pool->alivenum - pool->minnum);
            pool->cv.notify_all();
        }
    }
}

void *ThreadPool::workerfun(void *args)
{
    ThreadPool *pool = static_cast<ThreadPool *>(args);
    std::function<void()> task;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(pool->mtx);
            // 线程池开启且没有任务时阻塞
            while (!pool->isstop && pool->TaskQue->getsize() == 0)
            {
                pool->cv.wait(lock);
                // 在被唤醒后，判断是否要销毁
                if (pool->destorynum > 0)
                {
                    pool->killthread();
                    pool->destorynum--;
                    pool->alivenum--;
                }
            }
            // 线程池关闭了，且没有任务，将唤醒的线程自杀
            if (pool->isstop && pool->TaskQue->getsize() == 0)
            {
                pool->killthread();
            }

            // 取出任务，执行
            task = pool->TaskQue->gettask();
            pool->busynum++;
        }
        // 执行任务函数
        task();
        // 执行完之后，忙碌线程数要减一
        std::unique_lock<std::mutex> lock(pool->mtx);
        pool->busynum--;
    }
}

void ThreadPool::addtask_thread(std::function<void()> task)
{
    // 先判断线程池是否关闭
    if (isstop)
        return;
    TaskQue->addtask(task);
    cv.notify_one();
}

void ThreadPool::killthread()
{
    // 获取到线程ID
    pthread_t IDnum = pthread_self();

    for (int i = 0; i < maxnum; i++)
    {
        if (pthreadID[i] == IDnum)
        {
            pthreadID[i] = 0;
            break;
        }
    }
    // 退出当前线程
    pthread_exit(NULL);
}