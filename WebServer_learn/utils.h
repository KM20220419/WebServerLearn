#ifndef UTILS_H
#define UTILS_H
#include <fcntl.h>

class utils
{
public:
    void printerror(bool connection, char *message);
    void setnonblock(int fd);

public:
    int port = 9006;
};

#endif
