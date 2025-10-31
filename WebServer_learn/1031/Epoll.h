#pragma once
#include <sys/epoll.h>

#define MAX_EVENT 1024
class Epoll
{
public:
    Epoll();
    ~Epoll();
    void addepoll(int fd, uint32_t op);
    void modepoll(int fd, uint32_t op);
    void delfd(int fd);
    std::vector<epoll_event> poll(int timeout);

private:
    int epollfd;
    struct epoll_event *events;
};
