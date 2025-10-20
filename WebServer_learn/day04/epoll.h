#ifndef EPOLL_H
#define EPOLL_H
#include "../utils.h"
#include <sys/epoll.h>
#include <vector>
#define MAX_EVENT 128
class epoll
{
public:
    epoll();
    ~epoll();
    int create();
    void add_event(int fd);
    std::vector<epoll_event> poll(int timeout);

private:
    utils util;
    int epollfd;
    struct epoll_event events[MAX_EVENT], ev;
};

#endif