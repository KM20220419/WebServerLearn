#ifndef SERVER_H
#define SERVER_H
#include "../utils.h"

class sockaddress;

class server
{
public:
    server(int listen_num);
    ~server();
    void initsock();
    void bind(sockaddress *address);
    void listen();
    int accept(sockaddress *address);
    int getfd();

private:
    utils util;
    int MAX_LISTEN;
    int listenfd;
};
#endif