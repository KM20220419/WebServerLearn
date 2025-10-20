#include <sys/socket.h>
#include <iostream>
// 使用socketaddr_in 要包含这个结构体
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include "utils.h"

int main()
{
    utils util;
    // 定义一个监听socket TCP
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr;
    // 清空
    memset(&sockaddr, 0, sizeof(sockaddr));
    // 定义监听的地址
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(util.port);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 端口复用
    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(listenfd, 128);

    // 接受连接
    struct sockaddr_in client_address;
    socklen_t client_add_len = sizeof(client_address);
    bzero(&client_address, sizeof(client_address));
    int client_fd = accept(listenfd, (struct sockaddr *)&client_address, &client_add_len);
    util.printerror((client_fd == -1), "accept failture");

    int i = 0;
    while (true)
    {
        char send_buf[1024];
        char recv_buf[1024];
        memset(send_buf, 0, sizeof(send_buf));
        memset(recv_buf, 0, sizeof(recv_buf));
        ssize_t read_ret = recv(client_fd, recv_buf, sizeof(recv_buf), 0);
        if (read_ret <= 0)
        {
            perror("recv error");
            break;
        }
        std::cout << "服务端接收:" << recv_buf << std::endl;
        sprintf(send_buf, "第%d次", i + 1);
        i++;
        if (i >= 10)
            break;
        ssize_t send_ret = send(client_fd, &send_buf, sizeof(send_buf), 0);
        if (send_ret <= 0)
        {
            perror("send error");
            break;
        }
        std::cout << "服务端发送:" << send_buf << std::endl;
    }
    close(client_fd);
    close(listenfd);
    return 0;
}