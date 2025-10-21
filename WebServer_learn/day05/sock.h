#ifndef SOCK_H
#define SOCK_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "../utils.h"

class Inetaddr;

class Socket
{
public:
    Socket();
    ~Socket();
    void initsock();
    void bind(Inetaddr *sockaddress);
    void listen();
    ssize_t accept(Inetaddr *sockaddress);
    ssize_t getlistenfd();

private:
    utils util;
    ssize_t listenfd;
};

#endif