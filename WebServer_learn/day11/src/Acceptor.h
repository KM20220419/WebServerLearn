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
    void setnewconnection(std::function<void(Socket *)> newaccept);
    void acceptnewconnection();
    void startlisten();

private:
    Socket *listensock;
    InterAdd *maininter;
    Eventloop *mainloop;
    Channel *listench;
    std::function<void(Socket *)> newconnection;
};