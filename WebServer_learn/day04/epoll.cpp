#include "epoll.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

epoll::epoll()
{
    epollfd = -1;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));
}

epoll::~epoll()
{
    if (epollfd != -1)
    {
        close(epollfd);
        epollfd = -1;
    }
}
int epoll::create()
{
    epollfd = epoll_create1(0);
    util.printerror(epollfd == -1, "epoll create error");
    return epollfd;
}
void epoll::add_event(int fd)
{
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    ssize_t ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    util.printerror(ret == -1, "epoll add error");
}

std::vector<epoll_event> epoll::poll(int timeout)
{
    std::vector<epoll_event> ready_events;
    int fds = epoll_wait(epollfd, events, MAX_EVENT, timeout);
    util.printerror(fds == -1, "epoll wait error");
    for (int i = 0; i < fds; i++)
    {
        ready_events.push_back(events[i]);
    }
    return ready_events;
}