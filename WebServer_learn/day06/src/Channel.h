#pragma once

#include <sys/types.h>
#include <cstdint>
#include <functional>
class Eventloop;
class Channel
{
public:
    Channel(Eventloop *eloop, int fd_);
    ~Channel();
    int getfd();
    uint32_t getevent();
    uint32_t getrevent();
    bool getinepoll();
    void setevent(uint32_t op);
    void setrevent(uint32_t op);
    void setinepoll();
    void funct();
    void setfunc(std::function<void()> fun);

private:
    Eventloop *eventloop;
    int fd;
    uint32_t event;
    uint32_t revent;
    bool inepoll;
    std::function<void()> func;
};