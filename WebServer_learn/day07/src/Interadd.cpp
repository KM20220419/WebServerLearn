#include <string.h>

#include "Interadd.h"

InterAdd::InterAdd()
{
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddlen = 0;
}

InterAdd::InterAdd(const char *ip, int port)
{
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ip);
    sockaddlen = sizeof(sockaddr);
}

InterAdd::~InterAdd() {};
