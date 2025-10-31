#include <iostream>
#include <unistd.h> // sleep
#include <functional>
#include "timer.h" // 你的定时器头文件

using namespace std;

// 模拟关闭连接函数
void closeConn(int fd)
{
    cout << "[closeConn] fd = " << fd << " 被关闭" << endl;
}

int main()
{
    HeapTimer timer;

    // 添加三个定时任务
    timer.addNode(1, 2000, std::bind(closeConn, 1)); // 2 秒后关闭 fd=1
    timer.addNode(2, 4000, std::bind(closeConn, 2)); // 4 秒后关闭 fd=2
    timer.addNode(3, 6000, std::bind(closeConn, 3)); // 6 秒后关闭 fd=3

    cout << "=== 定时器测试开始 ===" << endl;

    int timePassed = 0;
    while (timePassed <= 8)
    {                                   // 总共运行 8 秒
        int next = timer.getNextTick(); // 下一个到期时间
        if (next == 0)
            next = 1000; // 没任务时每秒检查一次

        usleep(next * 1000); // 等待
        timePassed += next / 1000;

        // // 检查是否有超时任务
        // timer.tick();

        cout << "当前时间：" << timePassed << "s" << endl;
    }

    cout << "=== 测试结束 ===" << endl;
    return 0;
}
