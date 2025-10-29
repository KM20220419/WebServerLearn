#include "iostream"
#include <string>

#include "./src/ThreadPool.hpp"

void print(int a, double b, const char *c, std::string d)
{
    std::cout << a << b << c << d << std::endl;
}

void test()
{
    std::cout << "hellp" << std::endl;
}

int main()
{
    ThreadPool *pool = new ThreadPool(8); // 最大线程数为8
    auto feature = pool->task(print, 12, 19, "Hello ", std::string("World"));
    auto task_test = pool->task(test);

    delete pool;
    return 0;
}