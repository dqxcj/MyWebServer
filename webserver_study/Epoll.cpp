#include "Epoll.h"

/**
 * @description: 封装了epoll_create1函数
 * @param {int} flags epoll_create1函数的flags参数
 * @param {int} max_events_num 最多监听事件数
 * @return {*}
 */
Epoll::Epoll(int flags, int max_events_num): 
    max_events_num_(max_events_num),
    events_(max_events_num_) {
    // 新建epoll
    epfd_ = epoll_create1(flags);
    ErrIf(epfd_ == -1, "server error: epoll_create1()");

    // 初始化为0
    for(auto &ev : events_) {
        memset(&ev, 0, sizeof(ev));
    }
}

/**
 * @description: 封装了epoll_ctl函数的ADD功能
 * @param {int} fd 要操作的文件描述符
 * @param {uint32_t} events_flag 事件类型
 * @param {bool} is_no_block 是否将fd设置为非阻塞
 * @return {void}
 */
void Epoll::AddFd(int fd, uint32_t events_flag, bool is_no_block) {
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = events_flag;                                      // 模式
    if(is_no_block) {
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); // 设置成非阻塞
    }
    ErrIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "server error: epoll_ctl()");
}

/**
 * @description: 封装了epoll_wait函数
 * @param {int} timeout 超时参数
 * @return {std::vector<epoll_event>}
 */
// std::vector<epoll_event> Epoll::Wait(int timeout) {
//     int ev_num = epoll_wait(epfd_, events_.data(), max_events_num_, timeout);
//     ErrIf(ev_num == -1, "server error: epoll_wait()");

//     return std::vector<epoll_event>(events_.begin(), events_.begin() + ev_num);
// }

/**
 * @description: 封装了epoll_wait函数
 * @param {int} timeout 超时参数
 * @return {std::vector<Channel *>}
 */
std::vector<Channel *> Epoll::Wait(int timeout) {
    int ev_num = epoll_wait(epfd_, events_.data(), max_events_num_, timeout);
    ErrIf(ev_num == -1, "server error: epoll_wait()");

    std::vector<Channel *> res;
    for(int i = 0; i < ev_num; i++) {
        Channel *channel = (Channel *)events_[i].data.ptr;
        channel->SetTheMomentEvents(events_[i].events);
        res.push_back(channel);
    }
    return res;
}

/**
 * @description: 将含有指向channel的指针的event加入监听，若已加入，则依据channel->events进行更新
 * @param {Channel *} channel 一个指向channel的指针
 * @return {void}
 */
void Epoll::UpdateChannel(Channel *channel) {
    int fd = channel->GetFd();
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->GetEvents();   // 获取当前的events，一般已被修改，与原先的不同。借此来修改监听的事件
    if(!channel->GetIsInEpoll()) {      // 新增
        ErrIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "server error: UpdateChannel()->epoll_ctl()");
        channel->SetIsInEpoll(true);
    } else {                            // 修改
        ErrIf(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "server error: UpdateChannel()->epoll_ctl()");
    }
}

