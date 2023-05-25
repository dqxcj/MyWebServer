#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
public:
    Acceptor(EventLoop *loop);
    ~Acceptor();

    void HandleNewConnection();
    void SetNewConnectionCallBack(std::function<void(Socket *)> &&new_connection_call_back);

private:
    EventLoop *loop_;
    Socket *serv_sock_;
    Channel *acceptor_channel_;
    std::function<void(Socket *)> new_connection_call_back_;
};

#endif