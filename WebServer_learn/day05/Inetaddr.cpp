#include "Inetaddr.h"
#include <string.h>

Inetaddr::Inetaddr()
{
    // bzero(&sockaddr, addrlen); 这里的长度不要写addrlen，因为还没有初始化,或者将addrelen初始化放在前面
    // 最好使用siezof()

    // 你在构造函数里 先用 addrlen 初始化 sockaddr
    // 但此时 addrlen 还没被赋值，它的值是未定义的（随机垃圾值）
    // bzero 会尝试清零 addrlen 字节，可能非常大 → 访问非法内存 → 段错误
    // 不要用未初始化的值作为长度
    bzero(&sockaddr, sizeof(sockaddr));
    addrlen = 0;
}
// Inetaddr::Inetaddr(int)
// {
//     bzero(&sockaddr, addrlen);
//     addrlen = 0;
// }
void Inetaddr::setsockaddr(const char *ip, int port_)
{
    bzero(&sockaddr, addrlen);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port_);
    sockaddr.sin_addr.s_addr = inet_addr(ip);
    addrlen = sizeof(sockaddr);
}
