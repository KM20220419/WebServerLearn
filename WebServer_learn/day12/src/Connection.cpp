#include <string.h>
#include <iostream>
#include <functional>
#include <sys/epoll.h>
#include "Sock.h"
#include "Buf.h"
#include "Channel.h"
#include "Interadd.h"
#include "Connection.h"

Connection::Connection(Eventloop *loop_, Socket *sock) : mainloop(loop_), clientsock(sock)
{
    ConnectionCh = new Channel(mainloop, clientsock->getfd());
    ConnectionCh->setevent(EPOLLIN | EPOLLET);
    std::function<void()> newfun = std::bind(&Connection::readevent, this, clientsock->getfd());
    ConnectionCh->setfunc(newfun);
    recv_buf = new Buf();
    send_buf = new Buf();
}

Connection::~Connection()
{
    delete recv_buf;
    delete send_buf;
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
            recv_buf->appendstr(buf, sizeof(buf));
        }
        else if (recv_bytes == -1 && errno == EINTR)
        {
            // EINTR被系统调用中断
            continue;
        }
        else if (recv_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            char *sendtempbuf = "Data send";
            send_buf->appendstr(sendtempbuf, sizeof(sendtempbuf));
            std::cout
                << "服务器接收完毕" << std::endl;
            ssize_t send_bytes = send(fd, send_buf->buf_c_str(), send_buf->buf_size(), 0);
            recv_buf->cleanbuf();
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