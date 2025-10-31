#pragma once

#include <stdlib.h>
#include <chrono>
#include <vector>
#include <functional>
class TimerNode
{
public:
    TimerNode(int id_, std::chrono::steady_clock::time_point expiretime_, const std::function<void()> &cb) : id(id_), expiretime(expiretime_), timeoutcb(cb) {};

public:
    int id;
    std::chrono::steady_clock::time_point expiretime;
    std::function<void()> timeoutcb;

    // 使用const避免改变内容，使用&则是避免再次复制一份临时对象，导致效率低下
    bool operator<(const TimerNode &t1)
    {
        return expiretime < t1.expiretime;
    }
    bool operator>(const TimerNode &t1)
    {
        return expiretime > t1.expiretime;
    }
};

class HeapTimer
{
public:
    HeapTimer();
    ~HeapTimer();

    void adjustExpire(int id, int newexpiretime);
    void addNode(int id_, int timeout, const std::function<void()> &cbfun);
    void workfunc(int id_);
    void deleteNode(size_t i);

    // 清理超时的定时器
    void tick();

    // 清空容器
    void clear();

    size_t getNextTick();

private:
    void UpNode(size_t i);
    bool DownNode(size_t i, size_t n);
    // 传入堆中的下标，并交换
    void SwapNode(size_t i, size_t j);

    std::vector<TimerNode> timer;
    // 定义这个，可以根据ID找到节点在堆中的下标
    std::unordered_map<int, size_t> ref;
};