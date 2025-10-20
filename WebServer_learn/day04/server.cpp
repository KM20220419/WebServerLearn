#include "webserver.h"
#include "epoll.h"
#include "sockaddr.h"
#include <vector>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "../utils.h"

int main()
{
    server *wserver = new server(5);
    sockaddress *socka = new sockaddress();
    wserver->initsock();
    socka->add_sockaddr("127.0.0.1", 9006);
    wserver->bind(socka);
    wserver->listen();
    utils *util = new utils();
    int listenfd = wserver->getfd();
    util->setnonblock(listenfd);

    epoll *ep = new epoll();
    int epollfd = ep->create();
    ep->add_event(listenfd);
    while (true)
    {
        std::vector<epoll_event> readyepoll = ep->poll(-1);
        for (int i = 0; i < readyepoll.size(); i++)
        {
            if (readyepoll[i].data.fd == listenfd)
            {
                sockaddress clientaddr;
                int clientfd = wserver->accept(&clientaddr);
                util->setnonblock(clientfd);
                ep->add_event(clientfd);
            }
            else if (readyepoll[i].events & EPOLLIN)
            {
                // 处理数据 ET
                char buf[1024];
                bzero(buf, sizeof(buf));
                while (true)
                {
                    ssize_t recv_bytes = recv(readyepoll[i].data.fd, buf, sizeof(buf), 0);
                    if (recv_bytes > 0)
                    {
                        std::cout << "服务器接收到了，发送：" << buf << std::endl;
                        ssize_t send_bytes = send(readyepoll[i].data.fd, buf, recv_bytes, 0);
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
                        close(readyepoll[i].data.fd);
                        break;
                    }
                }
            }
            else
            {
                // 别的事务
            }
        }
    }
    delete wserver;
    delete socka;
    return 0;
}