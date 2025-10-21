#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "epoll.h"
#include "../utils.h"
#include "Channel.h"
#define MAXEVENT 1000
myepoll::myepoll() : epollfd(-1), events(nullptr)
{
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll create error");
    }
    events = new epoll_event[MAXEVENT];
    bzero(events, sizeof(*events) * MAXEVENT);
}
myepoll::~myepoll()
{
    if (epollfd != -1)
    {
        close(epollfd);
        epollfd = -1;
    }
    delete[] events;
}
void myepoll::addfd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.events = op;
    ev.data.fd = fd;

    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl error");
        return;
    }
    return;
}

void myepoll::updatechannel(Channel *channel)
{
    int fd = channel->getfd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    // 关键--ev中加入的不再是fd，而是Channel对象指针
    ev.data.ptr = channel;

    ev.events = channel->getevent();
    if (!channel->getinepoll())
    {
        int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        util.printerror(ret == -1, "epoll add error");
        channel->setinepoll();
        return;
    }
    else
    {
        int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        util.printerror(ret == -1, "epoll mod error");
        return;
    }
}

std::vector<Channel *> myepoll::poll(int timeout)
{
    // 定义一个存放Channel类型的指针
    std::vector<Channel *> readyChannel;
    int fds = epoll_wait(epollfd, events, MAXEVENT, timeout);
    util.printerror(fds == -1, "epoll wait error");
    for (int i = 0; i < fds; i++)
    {
        // 取出之前注册的Channel*
        Channel *ch = (Channel *)events[i].data.ptr;
        ch->setrevent(events[i].events);
        readyChannel.push_back(ch);
    }
    return readyChannel;
}