#pragma once
#include <queue>
#include <mutex>
#include <functional>
class TaskQueue
{
public:
    void addtask(std::function<void()> taskfunc);
    std::function<void()> gettask();
    int getsize();

private:
    std::mutex mtx;
    std::queue<std::function<void()>> taskque;
};