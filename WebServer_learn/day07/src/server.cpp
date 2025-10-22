#include <string.h>
#include <iostream>
#include <unistd.h>
#include "server.h"
#include "Sock.h"
#include "Epoll.h"
#include "Interadd.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Acceptor.h"

server::server(Eventloop *loop) : eventloop(loop)
{
    Acceptor *accept = new Acceptor(eventloop);

    // 这里使用_1作为一个占位符，其参数随函数的调用传入，因此返回的函数类型为void(Socket*)
    // std::function<void()> listenfun = std::bind(&server::NewConnection, this, sock);
    // 这里固定了参数为sock，因此函数类型为void()
    std::function<void(Socket *)> listenfun = std::bind(&server::NewConnection, this, std::placeholders::_1);

    // 将接受连接的函数交给accept保存
    accept->acceptfun(listenfun);

    // 延迟监听，没有和构造写在一起。因为构造的时候，回调函数还没设置完成。
    accept->startlisten();
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
            perror("FPY: recv error");
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