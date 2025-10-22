#pragma once

#include <sys/socket.h>
#include "utils.h"
class InterAdd;
class Socket
{
public:
    Socket();
    Socket(int);
    ~Socket();
    void initsock();
    void bind(InterAdd *address);
    void listen();
    void nonblock();
    ssize_t accept(InterAdd *address);
    ssize_t getfd();

private:
    utils util;
    ssize_t listenfd;
};