#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>

class Epoll;

class Channel {
public:
    Channel(int fd, Epoll *ep);

    int GetFd();
    uint32_t GetEvents();
    uint32_t GetTheMomentEvents();
    void SetTheMomentEvents(uint32_t the_moment_events);
    bool GetIsInEpoll();
    void SetIsInEpoll(bool is_in_epoll);
    void EnableRead();
private:
    int fd_;                      // 对应的文件描述符
    Epoll *ep_;                   // 对应的epoll
    uint32_t events_;              // 要监听的事件类型
    uint32_t the_moment_events_;  // 在该fd上正在发生的事件类型，也就是调用epoll_wait时该fd上发生的事件类型
    bool is_in_epoll_;            // 该fd是否加入了epoll的事件表
};

#endif