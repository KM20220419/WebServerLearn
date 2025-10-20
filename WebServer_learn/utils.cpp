#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// #include <fcntl.h>

void utils::printerror(bool connection, char *message)
{
    if (connection)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}
void utils::setnonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        perror("F_GEFL error");
        exit(1);
    }
    flag |= O_NONBLOCK;
    flag = fcntl(fd, F_SETFL, flag);
    if (flag == -1)
    {
        perror("F_SETFL error");
        exit(1);
    }
    return;
}