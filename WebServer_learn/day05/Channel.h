#pragma once

#include <cstdint>
class myepoll;
class Channel
{
public:
    Channel(myepoll *ep_, int fd_);
    ~Channel();
    int getfd();
    uint32_t getevent();
    uint32_t getrevent();
    bool getinepoll();
    void setrevent(uint32_t ev_);
    void setinepoll();
    void addevent(uint32_t op);

private:
    myepoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inepoll;
};
