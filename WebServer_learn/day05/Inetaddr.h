#ifndef INETADDR_H
#define INETADDR_H
#include <arpa/inet.h>

class Inetaddr
{
public:
    Inetaddr();
    // Inetaddr(int);
    ~Inetaddr() {};
    void setsockaddr(const char *ip, int port_);
    struct sockaddr_in sockaddr;
    socklen_t addrlen;
};

#endif