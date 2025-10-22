#pragma once
#include <functional>

class Eventloop;
class Channel;
class Socket;
class InterAdd;
class Acceptor
{
public:
    Acceptor(Eventloop *loop_);
    ~Acceptor();
    void setnewconnection();
    void acceptfun(std::function<void(Socket *)> fun);
    void startlisten();

private:
    Socket *listensock;
    InterAdd *maininter;
    Eventloop *mainloop;
    Channel *acceptch;
    std::function<void(Socket *)> cd;
};