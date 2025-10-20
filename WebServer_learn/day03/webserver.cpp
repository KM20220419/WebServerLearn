#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "../utils.h"

#define MAX_LISTEN 128
#define MAX_EVENTS 1024
int main()
{
    utils util;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // 定义地址
    struct sockaddr_in sockaddr;
    // 先清空
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(util.port);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ssize_t bind_ret = bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    util.printerror((bind_ret == -1), "bind error");

    // 端口复用
    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    ssize_t listen_ret = listen(listenfd, MAX_LISTEN);
    util.printerror((listen_ret == -1), " bind error");

    // 使用epoll
    int epollfd = epoll_create1(0);

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));

    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;

    util.setnonblock(listenfd);

    ssize_t ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    util.printerror((ret == -1), "epoll error");

    while (true)
    {
        int fns = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        util.printerror((fns == -1), "epoll_wait error");
        for (int i = 0; i < fns; i++)
        {
            if (events[i].data.fd == listenfd)
            {
                // 有新连接
                struct sockaddr_in client_sockaddr;
                bzero(&client_sockaddr, sizeof(client_sockaddr));
                socklen_t cli_addr_len = sizeof(client_sockaddr);
                int clientfd = accept(listenfd, (struct sockaddr *)&client_sockaddr, &cli_addr_len);
                util.printerror(clientfd == -1, "client accept error");

                util.setnonblock(clientfd);

                bzero(&ev, sizeof(ev));
                ev.data.fd = clientfd;
                ev.events = EPOLLIN | EPOLLET;
                int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                util.printerror(ret == -1, "client epoll_add error");
            }
            else if (events[i].events & EPOLLIN) // 可读事件
            {
                char buf[1024];
                bzero(buf, sizeof(buf));
                while (true)
                {
                    ssize_t recv_bytes = recv(events[i].data.fd, buf, sizeof(buf), 0);
                    if (recv_bytes > 0)
                    {
                        std::cout << "服务器接收到了，发送：" << buf << std::endl;
                        ssize_t send_bytes = send(events[i].data.fd, buf, recv_bytes, 0);
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
                        close(events[i].data.fd);
                        break;
                    }
                }
            }
            else
            { // 其他事件
                std::cout << "其他事件" << std::endl;
            }
        }
    }
    close(epollfd);
    close(listenfd);
    return 0;
}