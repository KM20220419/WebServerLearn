#pragma once

#include <sys/epoll.h>
#include <vector>
#include "utils.h"
#define MAXEVENT 1024
class Channel;
class Epoll
{
public:
    Epoll();
    ~Epoll();
    void addfd(int fd, uint32_t op);
    void updateChannel(Channel *);
    std::vector<Channel *> poll(int timeout);

private:
    utils util;
    ssize_t epollfd;
    struct epoll_event *events;
};