#include <sys/epoll.h>

#include "Sock.h"
#include "server.h"
#include "Interadd.h"
#include "Acceptor.h"
#include "Eventloop.h"
#include "Channel.h"
#include "Connection.h"

Acceptor::Acceptor(Eventloop *loop_) : mainloop(loop_)
{
    listensock = new Socket();
    maininter = new InterAdd("127.0.0.1", 9006);
    listensock->initsock();
    listensock->bind(maininter);
    listensock->listen();

    listensock->nonblock();
    listench = new Channel(mainloop, listensock->getfd());

    std::function<void()> usefun = std::bind(&Acceptor::acceptnewconnection, this);
    listench->setfunc(usefun);
}

Acceptor::~Acceptor()
{
    delete listensock;
    delete maininter;
    delete listench;
}

// 将监听独立出来，保证初始化的时候Channel 回调函数已经设置完成
void Acceptor::startlisten()
{
    listench->setevent(EPOLLIN | EPOLLET);
}

// 用于创建新连接时调用
void Acceptor::acceptnewconnection()
{
    InterAdd *clientaddr = new InterAdd();
    Socket *client_sock = new Socket(listensock->accept(clientaddr));
    client_sock->nonblock();
    newconnection(client_sock);
    delete clientaddr;
}

void Acceptor::setnewconnection(std::function<void(Socket *)> newaccept)
{
    newconnection = newaccept;
}
