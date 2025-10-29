#include " TaskQueue.h"

void TaskQueue::addtask(std::function<void()> taskfunc)
{
    std::unique_lock<std::mutex> lock(mtx);
    taskque.emplace(taskfunc);
}

std::function<void()> TaskQueue::gettask()
{
    std::function<void()> restask;
    if (taskque.empty())
        throw std::runtime_error("Taskque is empty");
    std::unique_lock<std::mutex> lock(mtx);
    restask = taskque.front();
    taskque.pop();
    return restask;
}

int TaskQueue::getsize()
{
    return taskque.size();
}