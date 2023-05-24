#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(int fd, EventLoop *loop): fd_(fd), loop_(loop), events_(0), the_moment_events_(0), is_in_epoll_(false) {}

int Channel::GetFd() {
    return fd_;
}
uint32_t Channel::GetEvents() {
    return events_;
}
uint32_t Channel::GetTheMomentEvents() {
    return the_moment_events_;
}
void Channel::SetTheMomentEvents(uint32_t the_moment_events) {
    the_moment_events_ = the_moment_events;
}
bool Channel::GetIsInEpoll() {
    return is_in_epoll_;
}
void Channel::SetIsInEpoll(bool is_in_epoll) {
    is_in_epoll_ = is_in_epoll;
}

/**
 * @description: 将当前channel监听的事件设置为可读，并且是ET模式，对应fd设置为非阻塞，并将channel添加到epoll的红黑树上
 * @return {void}
 */
void Channel::EnableRead() {
    events_ = EPOLLIN | EPOLLET;    // 可读 ET模式
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK); // 设置成非阻塞模式
    loop_->UpdateChannel(this);
}

void Channel::HandleEvent() {
    call_back_();
}

void Channel::SetCallBack(std::function<void()> &&call_back) {
    call_back_ = call_back;
}
