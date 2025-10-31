
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include "Epoll.h"

Epoll::Epoll() : epollfd(-1), events(nullptr)
{
    epollfd = epoll_create1(0);
    if (epollfd == -1)
        perror("epoll create");
    events = new epoll_event[MAX_EVENT];
    memset(&events, 0, sizeof(events));
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

void Epoll::addepoll(int fd, uint32_t op)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = op;
    ev.data.fd = fd;
    int res = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    if (res == -1)
    {
        perror("add epoll error");
        return;
    }
    return;
}

void Epoll::modepoll(int fd, uint32_t op)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events |= op;
    ev.data.fd = fd;
    int res = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    if (res == -1)
    {
        perror("epoll mod error");
        return;
    }
    return;
}

void Epoll::delfd(int fd)
{
    int res = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
    if (res == -1)
    {
        perror("fd delete error");
        return;
    }
    return;
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> Readyevent;
    int nfds = epoll_wait(epollfd, events, MAX_EVENT, timeout);
    if (nfds == -1)
    {
        perror("epoll wait error");
    }
    for (int i = 0; i < nfds; i++)
    {
        Readyevent.emplace_back(events[i]);
    }
    return Readyevent;
}