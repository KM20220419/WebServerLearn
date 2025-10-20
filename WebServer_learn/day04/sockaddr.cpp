#include <string.h>

#include "sockaddr.h"

sockaddress::sockaddress()
{
    bzero(&sock_addr, sizeof(sock_addr));
    addlen = 0;
}

void sockaddress::add_sockaddr(const char *ip, uint16_t port)
{
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = inet_addr(ip);
    addlen = sizeof(sock_addr);
    return;
}

sockaddress::~sockaddress() {};