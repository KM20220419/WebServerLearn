#pragma once

#include <map>
class Socket;
class Channel;
class Connection;
class Eventloop;
class Acceptor;
class server
{
public:
    server(Eventloop *loop);
    ~server();
    // 服务器类提供连接以及删除连接
    void NewConnection(Socket *newsock);
    void deleteconnection(Socket *);

private:
    Eventloop *eventloop;
    Acceptor *accept;
    std::map<int, Connection *> Connections;
};