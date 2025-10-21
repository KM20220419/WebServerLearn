#include <sys/socket.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "../utils.h"

int main()
{
    utils client_util;
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_sockaddr;
    memset(&client_sockaddr, 0, sizeof(client_sockaddr));

    client_sockaddr.sin_family = AF_INET;
    client_sockaddr.sin_port = htons(client_util.port);
    client_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t connect_ret = connect(client_sockfd, (struct sockaddr *)&client_sockaddr, sizeof(client_sockaddr));
    client_util.printerror((connect_ret == -1), "connect error");
    int i = 0;
    while (true)
    {
        char send_buf[1024];
        char recv_buf[1024];
        memset(send_buf, 0, sizeof(send_buf));
        memset(recv_buf, 0, sizeof(send_buf));

        sprintf(send_buf, "第%d次", i + 1);
        i++;
        if (i >= 10)
            break;
        ssize_t send_ret = send(client_sockfd, send_buf, sizeof(send_buf), 0);
        if (send_ret <= 0)
        {
            perror("send error");
            break;
        }
        std::cout << "客户端发送:" << send_buf << std::endl;
        ssize_t recv_ret = recv(client_sockfd, recv_buf, sizeof(recv_buf), 0);
        if (recv_ret == -1)
        {
            perror("recv error");
            break;
        }
        if (recv_ret == 0)
        {
            perror("client fd  close");
            break;
        }
        std::cout << "客户端接收:" << recv_buf << std::endl;
    }
    close(client_sockfd);
    return 0;
}