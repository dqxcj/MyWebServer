#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(int fd, std::shared_ptr<EventLoop> loop)
    : fd_(fd), loop_(loop), events_(0), the_moment_events_(0), is_in_epoll_(false), use_thread_pool_(true) {}

Channel::~Channel() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

int Channel::GetFd() { return fd_; }
uint32_t Channel::GetEvents() { return events_; }
uint32_t Channel::GetTheMomentEvents() { return the_moment_events_; }
void Channel::SetTheMomentEvents(uint32_t the_moment_events) { the_moment_events_ = the_moment_events; }
bool Channel::GetIsInEpoll() { return is_in_epoll_; }
void Channel::SetIsInEpoll(bool is_in_epoll) { is_in_epoll_ = is_in_epoll; }

/**
 * @description: 将当前channel监听的事件设置为可，并将channel添加到epoll的红黑树上
 * @return {void}
 */
void Channel::EnableRead() {
  events_ |= (EPOLLIN | EPOLLPRI);  // 可读
  loop_->UpdateChannel(this);
}

void Channel::UseET() {
  events_ |= EPOLLET;
  loop_->UpdateChannel(this);
}

void Channel::HandleEvent() {
  if (the_moment_events_ & (EPOLLIN | EPOLLPRI)) {
    if (use_thread_pool_)
      loop_->AddTask(read_call_back_);
    else
      read_call_back_();
  }

  if (the_moment_events_ & (EPOLLOUT)) {
    if (use_thread_pool_)
      loop_->AddTask(write_call_back_);
    else
      write_call_back_();
  }
}

void Channel::SetReadCallBack(std::function<void()> &&read_call_back) { read_call_back_ = read_call_back; }
void Channel::SetWriteCallBack(std::function<void()> &&write_call_back) { write_call_back_ = write_call_back; }
void Channel::SetUseThreadPool(bool use_thread_pool) { use_thread_pool_ = use_thread_pool; }
