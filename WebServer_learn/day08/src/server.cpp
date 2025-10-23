#include <string.h>
#include <iostream>
#include <unistd.h>
#include "server.h"
#include "Sock.h"
#include "Epoll.h"
#include "Interadd.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Acceptor.h"
#include "Connection.h"

server::server(Eventloop *loop) : eventloop(loop)
{
    accept = new Acceptor(eventloop);
    std::function<void(Socket *)> acceptnewConnection = std::bind(&server::NewConnection, this, std::placeholders::_1);
    accept->setnewconnection(acceptnewConnection);
    // 设置完回调函数，再监听
    accept->startlisten();
}

server::~server()
{
    delete accept;
}

void server::NewConnection(Socket *newsock)
{
    Connection *newconn = new Connection(eventloop, newsock);
    std::function<void(Socket *)> cb = std::bind(&server::deleteconnection, this, std::placeholders::_1);
    newconn->setdeleteconnection(cb);
    Connections[newsock->getfd()] = newconn;
}
void server::deleteconnection(Socket *deletesock)
{
    Connection *conn = Connections[deletesock->getfd()];
    Connections.erase(deletesock->getfd());
    delete conn;
};