#include <cassert>
#include <algorithm>
#include "timer.h"

HeapTimer::HeapTimer() {}
HeapTimer::~HeapTimer() {}

void HeapTimer::SwapNode(size_t i, size_t j)
{
    // size_t本来就表示非负值
    assert(i >= 0 && i < timer.size());
    assert(j >= 0 && j < timer.size());

    std::swap(timer[i], timer[j]);
    // 交换后，堆中第i个位置还在这，不过其对应的ID变了
    ref[timer[i].id] = i;
    ref[timer[j].id] = j;
}

void HeapTimer::UpNode(size_t i)
{
    assert(i < timer.size());
    // 注意下标从0开始
    int parent = (i - 1) / 2;
    while (parent >= 0)
    {
        if (timer[i] < timer[parent])
        {
            SwapNode(i, parent);
            i = parent;
            parent = (i - 1) / 2;
        }
        else
        {
            break;
        }
    }
}

// 节点下移为true
bool HeapTimer::DownNode(size_t i, size_t n)
{
    assert(i < timer.size());

    // 记录最开始的 用于对比，是否发生交换
    size_t temp = i;
    size_t child = i * 2 + 1;
    while (child < n)
    {
        if (child + 1 < n && timer[child + 1] < timer[child])
        {
            child++;
        }
        if (timer[child] < timer[i])
        {
            SwapNode(i, child);
            i = child;
            child = i * 2 + 1;
        }
        else
        {
            break;
        }
    }
    return temp < i;
}

void HeapTimer::deleteNode(size_t i)
{
    assert(i < timer.size());
    size_t n = timer.size() - 1;
    // 如果要删除的位置不是最后一个，就要和堆的最后一个进行交换
    if (i < timer.size() - 1)
    {
        SwapNode(i, n);
        // 此时交换完之后，调整前n -1 个位置的堆
        if (!DownNode(i, n))
            UpNode(i);
    }
    ref.erase(timer.back().id);
    timer.pop_back();
}

void HeapTimer::adjustExpire(int id, int newexpiretime)
{
    assert(!timer.empty() && ref.count(id));
    size_t index = ref[id];
    size_t n = timer.size();

    timer[ref[id]].expiretime = std::chrono::steady_clock::now() + std::chrono::milliseconds(newexpiretime);

    // 调整的过期时间可能减少也可能增加
    if (!DownNode(index, n))
        UpNode(index);
}

void HeapTimer::addNode(int id_, int timeout, const std::function<void()> &cbfun)
{
    assert(id_ >= 0);
    if (ref.count(id_))
    {
        size_t index = ref[id_];
        timer[index].timeoutcb = cbfun;
        timer[index].expiretime = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);
        if (!DownNode(index, timer.size()))
            UpNode(index);
    }
    else
    {
        std::chrono::steady_clock::time_point expiretime_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);
        TimerNode t1(id_, expiretime_, cbfun);
        timer.emplace_back(t1);
        ref[id_] = timer.size() - 1;
        UpNode(timer.size() - 1);
    }
}

void HeapTimer::workfunc(int id_)
{
    // 堆为空或者指定ID的值找不到
    if (timer.empty() || ref.count(id_) == 0)
        return;
    size_t index = ref[id_];

    // 执行函数
    timer[index].timeoutcb();

    // 执行完删除节点
    deleteNode(index);
}

void HeapTimer::tick()
{
    if (timer.empty())
        return;
    while (!timer.empty())
    {
        auto node = timer.front();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(node.expiretime - std::chrono::steady_clock::now()).count() <= 0)
        {
            node.timeoutcb();
            deleteNode(0);
        }
        break;
    }
}

void HeapTimer::clear()
{
    if (timer.empty())
        return;
    timer.clear();
    ref.clear();
}

size_t HeapTimer::getNextTick()
{
    tick();
    int res = -1;
    if (!timer.empty())
    {
        auto node = timer.front();
        res = std::chrono::duration_cast<std::chrono::milliseconds>(node.expiretime - std::chrono::steady_clock::now()).count();
    }
    if (res <= 0)
        res = 0;
    return res;
}