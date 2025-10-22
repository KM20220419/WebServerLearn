#pragma once

class Socket;
class Channel;
class Eventloop;
class server
{
public:
    server(Eventloop *loop);
    ~server();
    void HandleEvent(int fd);
    void NewConnection(Socket *newsock);

private:
    Socket *sock;
    Channel *listenChannel;
    Eventloop *eventloop;
};