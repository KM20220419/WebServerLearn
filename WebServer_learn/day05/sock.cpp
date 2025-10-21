#include <arpa/inet.h>
#include <unistd.h>

#include "sock.h"
#include "../utils.h"
#include "Inetaddr.h"

Socket::Socket()
{
    listenfd = -1;
}

Socket::~Socket()
{
    if (listenfd != -1)
    {
        close(listenfd);
        listenfd = -1;
    }
}

void Socket::initsock()
{
    listenfd = ::socket(AF_INET, SOCK_STREAM, 0);
    util.printerror(listenfd == -1, "init sock error");
}

void Socket::bind(Inetaddr *sockaddress)
{
    ssize_t bindret = ::bind(listenfd, (sockaddr *)&sockaddress->sockaddr, sockaddress->addrlen);
    util.printerror(bindret == -1, "bind error");
}

void Socket::listen()
{
    ssize_t listenret = ::listen(listenfd, 5);
    util.printerror(listenret == -1, "listen error");
}

ssize_t Socket::accept(Inetaddr *sockaddress)
{
    ssize_t clientfd = ::accept(listenfd, (sockaddr *)&sockaddress->sockaddr, &(sockaddress->addrlen));
    util.printerror(clientfd == -1, "accept error");
    return clientfd;
}

ssize_t Socket::getlistenfd()
{
    return listenfd;
}