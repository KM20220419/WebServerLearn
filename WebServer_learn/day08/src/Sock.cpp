#include <sys/socket.h>
#include <unistd.h>

#include "Interadd.h"
#include "utils.h"
#include "Sock.h"

Socket::Socket() : listenfd(-1) {};
Socket::Socket(int fd) : listenfd(fd) {};
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
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    util.printerror(listenfd == -1, "init socket error");
}

void Socket::bind(InterAdd *address)
{
    ssize_t bindret = ::bind(listenfd, (sockaddr *)&address->sockaddr, address->sockaddlen);
    util.printerror(bindret == -1, "bind error");
}

void Socket::listen()
{
    ssize_t listenret = ::listen(listenfd, 5);
    util.printerror(listenret == -1, "listen error");
}

void Socket::nonblock()
{
    util.printerror(listenfd == -1, "nonblock error");
    util.setnonblock(listenfd);
}
ssize_t Socket::accept(InterAdd *address)
{
    ssize_t clientfd = ::accept(listenfd, (sockaddr *)&address->sockaddr, &address->sockaddlen);
    util.printerror(clientfd == -1, "accept error");
    return clientfd;
}

int Socket::getfd()
{
    return listenfd;
}