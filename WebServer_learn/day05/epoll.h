#pragma once

#include <sys/epoll.h>
#include <vector>

#include "../utils.h"
class Channel;
class myepoll
{
public:
    myepoll();
    ~myepoll();
    void addfd(int fd, uint32_t op);
    void updatechannel(Channel *);
    std::vector<Channel *> poll(int timeout);

private:
    utils util;
    ssize_t epollfd;
    struct epoll_event *events;
};
