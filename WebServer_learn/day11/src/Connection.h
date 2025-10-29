#pragma once

class Buf;
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
    Buf *recv_buf;
    Buf *send_buf;
    Eventloop *mainloop;
    Socket *clientsock;
    Channel *ConnectionCh;
    std::function<void(Socket *)> deleteconnection;
};
