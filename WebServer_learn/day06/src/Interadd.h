#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

class InterAdd
{
public:
    InterAdd();
    InterAdd(const char *ip, int port);

    ~InterAdd();

    struct sockaddr_in sockaddr;
    socklen_t sockaddlen;
};
