#include <sys/epoll.h>

#include "Sock.h"
#include "server.h"
#include "Interadd.h"
#include "Acceptor.h"
#include "Eventloop.h"
#include "Channel.h"

Acceptor::Acceptor(Eventloop *loop_) : mainloop(loop_)
{
    listensock = new Socket();
    maininter = new InterAdd("127.0.0.1", 9006);
    listensock->initsock();
    listensock->bind(maininter);
    listensock->listen();

    listensock->nonblock();
    acceptch = new Channel(mainloop, listensock->getfd());
    // 不能这样写，因为这里的this指针要指向server，所以要在Acceptor中另写函数来封装
    //  std::function<void()> cb = std::bind(&server::NewConnection, this, listensock);

    // 使用this，会在对象使用时自动进行 accept->setnewconnection()
    std::function<void()> usefun = std::bind(&Acceptor::setnewconnection, this);
    acceptch->setfunc(usefun);
}

Acceptor::~Acceptor()
{
    delete listensock;
    delete maininter;
    delete acceptch;
}

// 将监听独立出来，保证初始化的时候回调函数已经设置完成
void Acceptor::startlisten()
{
    acceptch->setevent(EPOLLIN | EPOLLET);
}

void Acceptor::acceptfun(std::function<void(Socket *)> fun)
{
    cd = fun;
}

// 固定函数参数，将其类型转换为void()
void Acceptor::setnewconnection()
{
    cd(listensock);
}
