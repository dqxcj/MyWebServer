#ifndef SRC_INCLUDE_EVENTLOOP_H_
#define SRC_INCLUDE_EVENTLOOP_H_
#include <functional>
#include <vector>
#include <memory>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();
  void Loop();
  // 更新相应的event
  // 裸指针是为了更好地与内核交流
  void UpdateChannel(Channel *channel);
  void AddTask(std::function<void()> func);

 private:
  std::unique_ptr<Epoll> ep_;
  std::unique_ptr<ThreadPool> thread_pool_;
  bool stop_;
};

#endif  // SRC_INCLUDE_EVENTLOOP_H_
