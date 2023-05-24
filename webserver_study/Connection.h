#ifndef CONNECTION_H
#define CONNECTION_H
#include <functional>
#include <iostream>

#include <unistd.h>

class Socket;
class Channel;
class EventLoop;


class Connection {
public:
    Connection(EventLoop *loop_, Socket *clnt);
    ~Connection();
    void Echo(int rw_fd);
    void SetDeleteConnectionCallBack(std::function<void(Socket*)> delete_connection_call_back);
private:
    EventLoop *loop_;
    Channel *channel_;
    Socket *clnt_;
    std::function<void(Socket*)> delete_connection_call_back_;
};

#endif