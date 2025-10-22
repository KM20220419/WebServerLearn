#include "Channel.h"
#include "Eventloop.h"

Channel::Channel(Eventloop *eloop, int fd_) : eventloop(eloop), fd(fd_), inepoll(false), event(0), revent(0)
{
}

Channel::~Channel()
{
}

int Channel::getfd()
{
    return fd;
}

uint32_t Channel::getevent()
{
    return event;
}

uint32_t Channel::getrevent()
{
    return revent;
}

bool Channel::getinepoll()
{
    return inepoll;
}

void Channel::setinepoll()
{
    inepoll = true;
}

void Channel::setevent(uint32_t op)
{
    event = op;
    eventloop->updateChannel(this);
}

void Channel::setrevent(uint32_t op)
{
    revent = op;
}

// 执行Channel中的回调函数
void Channel::funct()
{
    func();
}

// 为Channel添加回调函数
void Channel::setfunc(std::function<void()> fun)
{
    func = fun;
}