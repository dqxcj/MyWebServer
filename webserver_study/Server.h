#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <functional>
#include <utility>

#include <unistd.h>

const int READ_BUFFER = 1024;

class EventLoop;
class Socket;

class Server {
public:
    Server(EventLoop *loop);

    void NewConnection(Socket *serv);
    void HandleReadEvent(int rw_fd);
private:
    EventLoop *loop_;
};

#endif