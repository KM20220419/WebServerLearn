
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "Epoll.h"
#include "Server.h"
#include "../1029--ThreadPool/ThreadPool.h"
#include "../1030--timer/timer.h"

Server::Server(int port_, const char *ip_) : port(port_), ip(ip_), timerstart(true)
{
    ep = new Epoll();
    thpool = new ThreadPool();
}

Server::~Server()
{
    if (listenfd != -1)
    {
        close(listenfd);
        listenfd = -1;
    }
    delete ep;
}

void Server::Initsocket()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sock;
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(ip);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    int bindres = bind(listenfd, (sockaddr *)&sock, sizeof(sock));
    if (bindres == -1)
    {
        perror("sock bind error");
    }

    int listenres = listen(listenfd, 5);
    if (listenres == -1)
    {
        perror("listen sock error");
    };
    ep->addepoll(listenfd, EPOLLIN | EPOLLET);
}

void Server::Start()
{
    while (true)
    {
        int timeout = -1;
        if (timerstart)
        {
            // 毫秒级的
            timeout = timer->getNextTick();
        }
        std::vector<epoll_event> Readyevents = ep->poll(timeout);
        for (int i = 0; i < Readyevents.size(); i++)
        {
            if (Readyevents[i].data.fd == listenfd)
            {
                Acceptor();
            }
            else if (Readyevents[i].events & EPOLLIN)
            {
                timer->adjustExpire(Readyevents[i].data.fd, 60000);
                thpool->addtask_thread(std::bind(&Server::Readevent, this, Readyevents[i].data.fd));
            }
            else
            {
                // 其他任务
            }
        }
    }
}

void Server::Acceptor()
{
    struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    socklen_t clientlen = sizeof(clientaddr);

    int connfd = accept(listenfd, (sockaddr *)&clientaddr, &clientlen);
    if (connfd == -1)
    {
        perror("accept error");
        return;
    }
    ep->addepoll(connfd, EPOLLIN | EPOLLET);
    timer->addNode(connfd, 60000, std::bind(&Server::closeConn, this, connfd));
}

void Server::closeConn(int fd)
{
    ep->delfd(fd);
    if (timerstart)
    {
        timer->deleteNode(fd);
    }
    close(fd);
}

void Server::Readevent(int fd)
{
    char buf[1024];
    while (true)
    {
        bzero(buf, sizeof(buf));
        int recv_bytes = recv(fd, buf, sizeof(buf), 0);
        if (recv_bytes > 0)
        {
            std::cout << "收到的信息为：" << buf << std::endl;
        }
        else if (recv_bytes == -1 && errno == EINTR)
        {
            // EINTR被系统调用中断
            continue;
        }
        else if (recv_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            std::cout << "服务器接收完毕" << std::endl;
            break;
        }
        else
        {
            perror("recv error");
            break;
        }
    }
}