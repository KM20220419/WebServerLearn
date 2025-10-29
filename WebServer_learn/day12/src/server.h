#pragma once

#include <map>
#include <vector>
class Socket;
class Channel;
class Connection;
class Eventloop;
class Acceptor;
class ThreadPool;
class server
{
public:
    server(Eventloop *loop);
    ~server();
    // 服务器类提供连接以及删除连接
    void NewConnection(Socket *newsock);
    void deleteconnection(Socket *);

private:
    Eventloop *mainloop;
    Acceptor *accept;
    std::map<int, Connection *> Connections;
    std::vector<Eventloop *> subRectors;

    ThreadPool *pool;
};