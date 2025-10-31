#include <sys/socket.h>

class Epoll;
class HeapTimer;
class ThreadPool;
class Server
{
public:
    Server(int port_, const char *ip_);

    void Initsocket();
    void Start();
    void Acceptor();
    void closeConn(int fd);
    void Readevent(int fd);

private:
    int port;
    const char *ip;
    int listenfd;
    bool timerstart;
    Epoll *ep;
    HeapTimer *timer;
    ThreadPool *thpool;
};