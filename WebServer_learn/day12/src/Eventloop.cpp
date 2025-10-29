#include "Epoll.h"
#include "Channel.h"
#include "Eventloop.h"

Eventloop::Eventloop() : ep(nullptr), quit(false)
{
    ep = new Epoll(); // 在这里引入epoll
};
Eventloop::~Eventloop()
{
    // 释放
    delete ep;
};

void Eventloop::updateChannel(Channel *ch)
{
    ep->updateChannel(ch);
}

void Eventloop::loop(int timeout)
{
    while (!quit)
    {
        std::vector<Channel *> ReadyChannel = ep->poll(timeout);
        for (auto it = ReadyChannel.begin(); it != ReadyChannel.end(); ++it)
        {
            // 执行Channel中的回调函数
            (*it)->funct();
        }
    }
}