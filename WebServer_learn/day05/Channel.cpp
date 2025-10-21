#include "epoll.h"
#include "Channel.h"

Channel::Channel(myepoll *ep_, int fd_) : ep(ep_), fd(fd_), events(0), revents(0), inepoll(false)
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
    return events;
}
uint32_t Channel::getrevent()
{
    return revents;
}

void Channel::setrevent(uint32_t ev_)
{
    revents = ev_;
}
bool Channel::getinepoll()
{
    return inepoll;
}
void Channel::setinepoll()
{
    inepoll = true;
}
void Channel::addevent(uint32_t op)
{
    events = op;
    ep->updatechannel(this);
}