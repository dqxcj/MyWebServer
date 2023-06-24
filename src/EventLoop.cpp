#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

EventLoop::EventLoop() : ep_(std::make_unique<Epoll>()), thread_pool_(std::make_unique<ThreadPool> (4)), stop_(true) {}

EventLoop::~EventLoop() {
  stop_ = true;
}

void EventLoop::Loop() {
  stop_ = false;
  while (!stop_) {
    std::vector<Channel *> channels = ep_->Wait(-1);
    for (auto &channel : channels) {
      channel->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *channel) { ep_->UpdateChannel(channel); }

void EventLoop::AddTask(std::function<void()> func) { thread_pool_->AddTask(std::move(func)); }
