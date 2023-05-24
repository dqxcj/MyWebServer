#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <functional>
#include <utility>
#include <unordered_map>

#include <unistd.h>



class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server {
public:
    Server(EventLoop *loop);
    ~Server();

    void NewConnection(Socket *clnt);
    void DeleteConnection(Socket *clnt);
    void HandleReadEvent(int rw_fd);
private:
    EventLoop *loop_;
    Acceptor *acceptor_;
    std::unordered_map<int, Connection *> tcp_connections_;
};

#endif