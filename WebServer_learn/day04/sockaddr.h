#ifndef SOCKADDR_H
#define SOCKADDR_H
#include <arpa/inet.h>
#include <sys/socket.h>
class sockaddress
{
public:
    sockaddress();
    ~sockaddress();
    void add_sockaddr(const char *ip, uint16_t port);

    struct sockaddr_in sock_addr;
    socklen_t addlen;
};

#endif