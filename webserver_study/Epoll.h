#ifndef EPOLL_H
#define EPOLL_H
#include "MyError.h"
#include "Channel.h"

#include <vector>

#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>

const int MAX_EVENTS_NUM = 1024;

class Epoll {
public:
    Epoll(int flags, int max_events_num = MAX_EVENTS_NUM);

    void AddFd(int fd, uint32_t events, bool is_no_block);

    // std::vector<epoll_event> Wait(int timeout);
    std::vector<Channel*> Wait(int timeout);

    void UpdateChannel(Channel *channel);


private:
    int epfd_;
    int max_events_num_;
    std::vector<epoll_event> events_;
};

#endif