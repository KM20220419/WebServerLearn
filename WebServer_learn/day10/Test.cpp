#include <iostream>
#include <string>
#include "./src/Threadpool.h"

void print(int a, double b, const char *c, std::string d)
{
    std::cout << a << b << c << d << std::endl;
}

void test()
{
    std::cout << "FPY" << std::endl;
}

int main(int argc, char const *argv[])
{
    Threadpool *poll = new Threadpool(8);
    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    poll->addtask(func);
    func = test;
    poll->addtask(func);
    delete poll;
    return 0;
}