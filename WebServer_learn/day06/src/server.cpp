#include <string.h>
#include <iostream>
#include <unistd.h>
#include "server.h"
#include "Sock.h"
#include "Epoll.h"
#include "Interadd.h"
#include "Channel.h"
#include "Eventloop.h"

server::server(Eventloop *loop) : eventloop(loop)
{
    sock = new Socket();                              // 监听套接字，一直有用，在析构释放
    InterAdd *inet = new InterAdd("127.0.0.1", 9006); // 绑定之后j就没用。可以释放
    sock->initsock();
    sock->bind(inet);
    delete inet;
    sock->listen();
    sock->nonblock();

    listenChannel = new Channel(eventloop, sock->getfd());
    listenChannel->setevent(EPOLLIN | EPOLLET);
    std::function<void()> listenfun = std::bind(&server::NewConnection, this, sock);
    listenChannel->setfunc(listenfun);
}

server::~server()
{
    delete sock;
    delete listenChannel;
    delete eventloop;
}

void server::HandleEvent(int fd)
{
    char buf[1024];
    while (true)
    {
        bzero(buf, sizeof(buf));
        int recv_bytes = recv(fd, buf, sizeof(buf), 0);
        if (recv_bytes > 0)
        {
            std::cout << "服务器接收到了，发送：" << buf << std::endl;
            ssize_t send_bytes = send(fd, buf, recv_bytes, 0);
        }
        else if (recv_bytes == -1 && errno == EINTR)
        {
            // EINTR被系统调用中断
            continue;
        }
        else if (recv_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            perror("recv error");
            break;
        }
        else if (recv_bytes == 0)
        {
            std::cout << "客户端关闭" << std::endl;
            close(fd);
            break;
        }
    }
}

void server::NewConnection(Socket *listensock)
{
    InterAdd inter;
    // ***这样写会在函数结束时析构套接字***
    // Socket newsoc(listensock->accept(&inter))
    // 符合面向对象思想 建立clientfd 套接字
    Socket *newsoc = new Socket(listensock->accept(&inter));
    newsoc->nonblock();

    Channel *ch = new Channel(eventloop, newsoc->getfd());
    ch->setevent(EPOLLIN | EPOLLET);
    std::function<void()> newfun = std::bind(&server::HandleEvent, this, newsoc->getfd());
    ch->setfunc(newfun);
}