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
#include "ThreadPool.hpp"

server::server(Eventloop *loop) : mainloop(loop)
{
    accept = new Acceptor(mainloop);

    std::function<void(Socket *)> acceptnewConnection = std::bind(&server::NewConnection, this, std::placeholders::_1);
    accept->setnewconnection(acceptnewConnection);

    int size = std::thread::hardware_concurrency(); // 获取硬件的线程数
    pool = new ThreadPool(size);
    for (int i = 0; i < size; i++)
    {
        subRectors.emplace_back(new Eventloop());
    }

    for (int i = 0; i < size; i++)
    {
        // 将不同的subRector中的loop函数，绑定到线程池中
        std::function<void()> subRector_task = std::bind(&Eventloop::loop, subRectors[i], -1);
        pool->task(subRector_task);
    }
    // 设置完回调函数，再监听
    accept->startlisten();
}

server::~server()
{
    delete accept;
}

void server::NewConnection(Socket *newsock)
{
    int random = newsock->getfd() % subRectors.size();
    Connection *newconn = new Connection(subRectors[random], newsock);
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