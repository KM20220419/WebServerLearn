#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>
#include "utils.h"

void utils::printerror(bool condition, const char *message)
{
    if (condition)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

void utils::setnonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    flag = fcntl(fd, F_SETFL, flag);
}