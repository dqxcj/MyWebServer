#ifndef EVENTLOOP_H
#define EVENTLOOP_H

class Epoll;
class Channel;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void Loop();
    void UpdateChannel(Channel *channel);
private:
    Epoll *ep_;
    bool stop_;
};

#endif