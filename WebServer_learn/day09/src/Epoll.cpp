#include "Epoll.h"
#include "Channel.h"
#include "utils.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

Epoll::Epoll() : epollfd(-1), events(nullptr)
{
    epollfd = epoll_create1(0);
    util.printerror(epollfd == -1, "epoll create error");
    events = new epoll_event[MAXEVENT];
}
Epoll::~Epoll()
{
    if (epollfd != -1)
    {
        close(epollfd);
        epollfd = -1;
    }
    delete[] events;
}

// 原版的 即不使用Channel类
void Epoll::addfd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = op;
    ev.data.fd = fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);

    util.printerror(ret == -1, "epoll_ctl error");
}

// 使用Channel
void Epoll::updateChannel(Channel *ch)
{
    int fd = ch->getfd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = ch->getevent(); // 通过ch->getevent() 拿到想要监听的事件，并给ev
    ev.data.ptr = ch;
    if (ch->getinepoll())
    {
        int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        util.printerror(ret == -1, "epoll mod error");
    }
    else
    {
        int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        util.printerror(ret == -1, "epoll add error");
        ch->setinepoll();
    }
}

// 将已经就绪的事件 Channel类 返回
std::vector<Channel *> Epoll::poll(int timeout)
{
    std::vector<Channel *> readyChannel;
    int fdns = epoll_wait(epollfd, events, MAXEVENT, timeout);
    util.printerror(fdns == -1, "epoll wait error");
    for (int i = 0; i < fdns; i++)
    {
        Channel *ch = (Channel *)events[i].data.ptr;
        ch->setrevent(events[i].events);
        readyChannel.push_back(ch);
    }
    return readyChannel;
}