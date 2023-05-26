#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "ThreadPool.h"

EventLoop::EventLoop(): ep_(new Epoll()), thread_pool_(new ThreadPool(4)), stop_(true) {}

EventLoop::~EventLoop() {
    delete ep_;
    delete thread_pool_;
    stop_ = true;
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

void EventLoop::AddTask(std::function<void()> func) {
    thread_pool_->AddTask(std::move(func));
}
