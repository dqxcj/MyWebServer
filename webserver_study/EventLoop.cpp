#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "../src/include/MyThreadPool/ThreadPool.h"

EventLoop::EventLoop(): ep_(new Epoll()), thread_pool_(new ThreadPool(2, 4)), stop_(true) {
    thread_pool_->SetStart();
}

EventLoop::~EventLoop() {
    delete ep_;
    thread_pool_->Close();
    delete thread_pool_;
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
