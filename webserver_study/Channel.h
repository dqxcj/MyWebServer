#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

#include <sys/epoll.h>
#include <fcntl.h>

class EventLoop;

class Channel {
public:
    Channel(int fd, EventLoop *loop);

    int GetFd();
    uint32_t GetEvents();
    uint32_t GetTheMomentEvents();
    void SetTheMomentEvents(uint32_t the_moment_events);
    bool GetIsInEpoll();
    void SetIsInEpoll(bool is_in_epoll);
    void EnableRead();

    void HandleEvent();
    void SetCallBack(std::function<void()> &&call_back);
private:
    int fd_;                      // 对应的文件描述符
    EventLoop *loop_;                   // 对应的EventLoop
    uint32_t events_;              // 要监听的事件类型
    uint32_t the_moment_events_;  // 在该fd上正在发生的事件类型，也就是调用epoll_wait时该fd上发生的事件类型
    bool is_in_epoll_;            // 该fd是否加入了epoll的事件表
    std::function<void()> call_back_;   // 回调函数，用于处理该channel对应的事件

};

#endif