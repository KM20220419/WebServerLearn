#pragma once

class Socket;
class Eventloop;
class Connection
{
public:
    Connection(Eventloop *loop_, Socket *clientsock);
    ~Connection();
    void readevent(int fd);
    void setdeleteconnection(std::function<void(Socket *)> cb);

private:
    Eventloop *mainloop;
    Socket *clientsock;
    Channel *ConnectionCh;
    std::function<void(Socket *)> deleteconnection;
};
