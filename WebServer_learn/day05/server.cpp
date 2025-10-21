#include <fcntl.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "sock.h"
#include "Inetaddr.h"
#include "epoll.h"
#include "Channel.h"
#include "../utils.h"

void setnonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    flag = fcntl(fd, F_GETFL, flag);
}
void handlerIN(Channel *ch)
{
    // 处理数据 ET
    char buf[1024];
    bzero(buf, sizeof(buf));
    while (true)
    {
        ssize_t recv_bytes = recv(ch->getfd(), buf, sizeof(buf), 0);
        utils util;
        if (recv_bytes > 0)
        {
            std::cout << "服务器接收到了，发送：" << buf << std::endl;
            ssize_t send_bytes = send(ch->getfd(), buf, recv_bytes, 0);
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
            close(ch->getfd());
            // 客户端断开时，同时释放其Channel
            delete ch;
            break;
        }
    }
}

int main()
{
    utils util;
    Socket *sock = new Socket();
    Inetaddr *inet = new Inetaddr();
    inet->setsockaddr("127.0.0.1", 9006);
    sock->initsock();
    int flag = 1;
    setsockopt(sock->getlistenfd(), SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sock->bind(inet);
    sock->listen();
    setnonblock(sock->getlistenfd());

    myepoll *epoll_1 = new myepoll();
    // 为listenfd 创建Channel 类，并添加事件
    Channel *listenChannel = new Channel(epoll_1, sock->getlistenfd());
    listenChannel->addevent(EPOLLIN | EPOLLET);

    while (true)
    {
        std::vector<Channel *> RChannel = epoll_1->poll(-1);
        int len = RChannel.size();
        for (int i = 0; i < len; i++)
        {
            if (RChannel[i]->getfd() == sock->getlistenfd())
            {
                Inetaddr acceptInet;
                ssize_t Clientfd = sock->accept(&acceptInet);

                util.printerror(Clientfd == -1, "accept error");
                setnonblock(Clientfd);

                Channel *ClientChannel = new Channel(epoll_1, Clientfd);
                ClientChannel->addevent(EPOLLIN | EPOLLET);
            }
            else if (RChannel[i]->getrevent() & EPOLLIN)
            {
                handlerIN(RChannel[i]);
            }
            else
            {
                std::cout << "other" << std::endl;
            }
        }
    }
    delete sock; // 析构函数中已经关闭了文件描述符
    delete inet;
    delete epoll_1;
    return 0;
}