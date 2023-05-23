#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

#include <vector>

EventLoop::EventLoop(): ep_(new Epoll()), stop_(true) {}

EventLoop::~EventLoop() {
    delete ep_;
}

void EventLoop::Loop() {
    stop_ = false;
    while(!stop_) {
        std::vector<Channel *> channels = ep_->Wait(-1);
        for(auto &channel : channels) {
            channel->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *channel) {
    ep_->UpdateChannel(channel);
}
