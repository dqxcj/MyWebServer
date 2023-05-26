#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <vector>
#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void Loop();
    void UpdateChannel(Channel *channel);
    void AddTask(std::function<void()> func);
private:
    Epoll *ep_;
    ThreadPool *thread_pool_;
    bool stop_;
};

#endif