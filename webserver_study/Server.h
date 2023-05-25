#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <functional>
#include <utility>
#include <unordered_map>
#include <vector>
#include <thread>

#include <unistd.h>



class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
public:
    Server(EventLoop *loop);
    ~Server();

    void NewConnection(Socket *clnt);
    void DeleteConnection(Socket *clnt);
    void HandleReadEvent(int rw_fd);
private:
    EventLoop *main_reactor_;   //主reactor 负责处理连接
    Acceptor *acceptor_;    // 连接器
    std::unordered_map<int, Connection *> tcp_connections_; //所有tcp连接
    std::vector<EventLoop *> sub_reactors_; // 副reactor 负责业务逻辑
    ThreadPool *thpoll; // 线程池
};

#endif