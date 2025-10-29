#include "ThreadPool.h"
#include "TaskQueue.h"
#include <string.h>

ThreadPool::ThreadPool()
{
    minnum = 2;
    maxnum = 8;
    busynum = 0;
    alivenum = 0;
    destorynum = 0;
    isstop = false;

    taskque = new TaskQueue();
    if (taskque == nullptr)
        throw std::runtime_error("TaksQueue create error");

    // 创建线程
    for (int i = 0; i < minnum; i++)
    {
        workthreads.emplace_back([this]()
                                 { this->workerthreadfun();
                                alivenum++; });
    }
    managerthread = std::thread([this]()
                                { this->managerthreadfun(); });
}

ThreadPool::ThreadPool(int min, int max) : minnum(min), maxnum(max)
{
    busynum = 0;
    alivenum = 0;
    destorynum = 0;
    isstop = false;

    taskque = new TaskQueue();
    if (taskque == nullptr)
        throw std::runtime_error("TaksQueue create error");

    for (int i = 0; i < minnum; i++)
    {
        workthreads.emplace_back([this]()
                                 { this->workerthreadfun(); });
        alivenum++;
    }
    managerthread = std::thread([this]()
                                { this->managerthreadfun(); });
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> stoplock(poolmtx);
        isstop = true;
    }
    cv.notify_all();
    for (auto &thread : workthreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    managerthread.join();
}

// 管理者线程
void ThreadPool::managerthreadfun()
{
    // 线程池开启，管理者线程就会一直执行
    while (!isstop)
    {
        // 每个3秒执行一次
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // 加锁操作
        std::unique_lock<std::mutex> lock(poolmtx);

        int tasknum = taskque->getqueue_size();

        // 如果存在空闲任务（即任务队列中有任务存在），并且线程数未满 -> 新建线程
        if (tasknum > 0 && alivenum < maxnum)
        {
            // 新建的线程数量为2，并且保证不超过最大线程数
            int addnum = std::min(2, maxnum - alivenum);
            for (int i = 0; i < addnum; i++)
            {
                workthreads.emplace_back([this]()
                                         { this->workerthreadfun(); });
                alivenum++;
            }
        }
        // 无空闲任务，并且忙碌的线程数没有可用线程一半 -> 减少线程
        if (busynum * 2 < alivenum && alivenum > minnum)
        {
            // 每次减少2个  如果可用线程与最小线程数相差没有2个，就销毁他们相差的数量
            destorynum = std::min(2, alivenum - minnum);
            // 唤醒线程，让他们自杀
            cv.notify_all();
        }
    }
}

void ThreadPool::workerthreadfun()
{
    while (true)
    {
        std::function<void()> threadtask;
        {
            std::unique_lock<std::mutex> lock(poolmtx);

            // 线程池未关闭，并且没有任务时阻塞
            while (!isstop && taskque->getqueue_size() == 0)
            {
                cv.wait(lock);
                // 这时，如果线程被唤醒，判断是否要销毁
                // 注意是在wait之后，进行判断（不能写在wait的条件判断之外）
                if (destorynum > 0)
                {

                    destorynum--;
                    alivenum--;
                    return;
                }
            }

            // 如果线程池关闭并且没有任务了，则返回 -> 关闭线程
            if (isstop && !(taskque->getqueue_size() == 0))
            {
                return;
            }
            if (destorynum > 0)
            {
                destorynum--;
                alivenum--;
                return;
            }
            threadtask = taskque->getTask();
            busynum++;
        }
        threadtask();

        // 函数执行完之后
        {
            std::unique_lock<std::mutex> lock(poolmtx);
            busynum--;
        }
    }
}

//  无法解决vector容器中被销毁线程的删去问题