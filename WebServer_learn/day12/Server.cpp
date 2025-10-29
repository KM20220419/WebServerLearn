#include "src/server.h"
#include "src/Eventloop.h"
#include "src/Epoll.h"

int main()
{
    Eventloop *mainloop = new Eventloop();
    server *mainserver = new server(mainloop);
    mainloop->loop(-1);
    // 注意析构顺序
    delete mainserver;
    delete mainloop;
    return 0;
}