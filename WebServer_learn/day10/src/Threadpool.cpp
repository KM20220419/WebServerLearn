#include <mutex>
#include "Threadpool.h"

Threadpool::Threadpool(int size_) : size(size_), stop(false)
{
    for (int i = 0; i < size; i++)
    {
        threads.emplace_back(std::thread([this]()
                                         {
            while(true)
            {
                std::unique_lock<std::mutex> ulock(taskmx);
                //当线程池停止或者有任务时，唤醒线程
                cv.wait(ulock, [this](){ return stop || !taskqueue.empty(); });
                if(stop && taskqueue.empty())
                    return;
                std::function<void()> task = taskqueue.front();
                taskqueue.pop();
                ulock.unlock();
                task();
            } }));
    }
}

Threadpool::~Threadpool()
{
    {
        std::unique_lock<std::mutex> stopmx(taskmx);
        stop = true;
    }
    cv.notify_all();
    // for (auto it = threads.begin(); it != threads.end(); it++)
    // {
    //     if ((*it).joinable())
    //         (*it).join();
    // }
    for (std::thread &th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
}

void Threadpool::addtask(std::function<void()> task)
{
    // 这里的任务函数类型被固定，都是无参的，所以在传入的时候用std::bind绑定了参数
    {
        std::unique_lock<std::mutex> queuemx(taskmx);
        if (stop)
        {
            throw std::runtime_error("Threadpool is stop");
        }
        // 这里会发生一次拷贝操作
        taskqueue.emplace(task);
    }
    cv.notify_one();
}