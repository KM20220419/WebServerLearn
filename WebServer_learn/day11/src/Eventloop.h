#pragma once

class Epoll;
class Channel;
class Eventloop
{
public:
    Eventloop();
    ~Eventloop();
    void updateChannel(Channel *);
    void loop(int tiemout);

private:
    Epoll *ep;
    bool quit;
};