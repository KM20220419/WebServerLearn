#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "webserver.h"
#include "sockaddr.h"

server::server(int listen_num) : MAX_LISTEN(listen_num)
{
    listenfd = -1;
}
server::~server()
{
    if (listenfd != -1)
    {
        close(listenfd);
        listenfd = -1;
    }
}

void server::initsock()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    util.printerror(listenfd == -1, "init socket error");
}

void server::bind(sockaddress *address)
{
    ssize_t bind_ret = ::bind(listenfd, (struct sockaddr *)&address->sock_addr, address->addlen);

    util.printerror(bind_ret == -1, "bind error");
}

void server::listen()
{
    ssize_t listen_ret = ::listen(listenfd, MAX_LISTEN);
    util.printerror(listen_ret == -1, "listen error");
}

int server::accept(sockaddress *address)
{
    int clientfd = ::accept(listenfd, (sockaddr *)&address->sock_addr, &address->addlen);
    util.printerror(clientfd == -1, "accept error");
    return clientfd;
}
int server::getfd()
{
    return listenfd;
}
