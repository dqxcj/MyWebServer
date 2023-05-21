#ifndef EPOLLER_H
#define EPOLLER_H

// 相关头文件 如epoller.h之于epoller.cc
// c库
#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
// cpp库
#include <vector>
// 其它库的.h文件
// 本项目的.h文件


const int kMaxEventsNums = 1024; // 每次的就绪事件的最大数量
const int kTimeOutMs = -1; // epoll_wait中的超时参数默认值，-1阻塞，0立即返回，或具体的等待时间，单位是毫秒

class Epoller {
public:
    // Epoller() = default; // 下面的构造函数使用了默认实参，这里不需要默认构造函数了
    explicit Epoller(int max_events_nums = kMaxEventsNums);
    ~Epoller();
    bool AddFd(int fd, uint32_t events);
    bool ModFd(int fd, uint32_t events);
    bool DelFd(int fd);
    int Wait(int time_out_ms = kTimeOutMs);
    int GetEventFd(size_t i) const;
    uint32_t GetEvents(size_t i) const; 

private:
    int epoll_fd_; // 指向内核事件表的文件描述符
    std::vector<struct epoll_event> epoll_events_; // 就绪事件集，每次调用epoll_wait时将就绪事件放入此数组
};

#endif