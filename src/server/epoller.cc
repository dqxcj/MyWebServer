#include "include/epoller.h"
using namespace std;

Epoller::Epoller(int max_events_nums): epoll_fd_(epoll_create(512)), epoll_events_(max_events_nums) {
    assert(epoll_fd_ >= 0);
    assert(epoll_events_.size() > 0);
}

Epoller::~Epoller() {
    close(epoll_fd_);
}

bool Epoller::AddFd(int fd, uint32_t events) {
    if(fd < 0) 
        return false;
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoller::ModFd(int fd, uint32_t events) {
    if (fd < 0) 
        return false;
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoller::DelFd(int fd) {
    if (fd < 0)
        return false;
    epoll_event ev;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) == 0;
}

int Epoller::Wait(int time_out_ms) {
    return epoll_wait(epoll_fd_, &epoll_events_[0], static_cast<int>(epoll_events_.size()), time_out_ms);
}

int Epoller::GetEventFd(size_t i) const {
    assert(i < epoll_events_.size());
    assert(i >= 0);
    return epoll_events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
    assert(i < epoll_events_.size());
    assert(i >= 0);
    return epoll_events_[i].events;
}

