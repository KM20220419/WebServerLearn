#include <error.h>

#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
}

TaskQueue::~TaskQueue()
{
}

void TaskQueue::addtask(std::function<void()> taskfunc)
{
    std::unique_lock<std::mutex> tasklock(mtx);
    // queue中使用emplace相比于push，可以减少临时对象的构造以及拷贝/移动
    taskQueue.emplace(taskfunc);
}

std::function<void()> TaskQueue::getTask()
{
    if (taskQueue.empty())
        throw std::runtime_error("TaskQueue is empty");
    std::unique_lock<std::mutex> tasklock(mtx);
    std::function<void()> task = taskQueue.front();
    taskQueue.pop();
    return task;
}

int TaskQueue::getqueue_size()
{
    // 获取队列长度时，也要加锁
    std::unique_lock<std::mutex> sizelock(mtx);
    return taskQueue.size();
}