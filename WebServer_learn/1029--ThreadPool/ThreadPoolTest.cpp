#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

// 模拟任务：打印线程 ID 和任务编号
void task_function(int task_id)
{
    std::cout << "[Task " << task_id << "] is running in thread "
              << std::this_thread::get_id() << std::endl;
    // 模拟计算负载
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[Task " << task_id << "] finished." << std::endl;
}

int main()
{
    srand((unsigned)time(nullptr));

    // 创建动态线程池：最小线程数 2，最大线程数 8
    ThreadPool *pool = new ThreadPool();

    std::cout << "=== 动态线程池测试开始 ===" << std::endl;

    // 第一波任务（触发线程创建）
    for (int i = 0; i < 10; ++i)
    {
        pool->addtask_thread(std::bind(task_function, i));
    }

    // 等待一段时间，观察扩容情况
    std::this_thread::sleep_for(std::chrono::seconds(11));

    std::cout << "\n>>> 第二波任务提交，触发再次扩容 <<<" << std::endl;
    for (int i = 10; i < 20; ++i)
    {
        pool->addtask_thread(std::bind(task_function, i));
    }

    // 再等待一段时间，让任务执行完并触发缩容
    std::this_thread::sleep_for(std::chrono::seconds(20));

    std::cout << "\n=== 动态线程池测试结束，准备销毁线程池 ===" << std::endl;

    delete pool;
    return 0;
}
