#ifndef SRC_INCLUDE_EVENTLOOP_H_
#define SRC_INCLUDE_EVENTLOOP_H_
#include <functional>
#include <vector>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();
  void Loop();
  void UpdateChannel(Channel *channel);
  void AddTask(std::function<void()> func);

 private:
  Epoll *ep_;
  ThreadPool *thread_pool_;
  bool stop_;
};

#endif  // SRC_INCLUDE_EVENTLOOP_H_
