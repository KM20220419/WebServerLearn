#include <string.h>
#include <iostream>
#include <functional>
#include <sys/epoll.h>
#include "Sock.h"
#include "Channel.h"
#include "Interadd.h"
#include "Connection.h"

Connection::Connection(Eventloop *loop_, Socket *sock) : mainloop(loop_), clientsock(sock)
{
    ConnectionCh = new Channel(mainloop, clientsock->getfd());
    ConnectionCh->setevent(EPOLLIN | EPOLLET);
    std::function<void()> newfun = std::bind(&Connection::readevent, this, clientsock->getfd());
    ConnectionCh->setfunc(newfun);
}

Connection::~Connection()
{
    delete clientsock;
    delete ConnectionCh;
}

void Connection::readevent(int fd)
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
            deleteconnection(clientsock);
            break;
        }
    }
}

void Connection::setdeleteconnection(std::function<void(Socket *)> cb)
{
    deleteconnection = cb;
}