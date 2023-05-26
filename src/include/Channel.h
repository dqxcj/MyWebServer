#ifndef SRC_INCLUDE_CHANNEL_H_
#define SRC_INCLUDE_CHANNEL_H_

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <functional>

class EventLoop;

class Channel {
 public:
  Channel(int fd, EventLoop *loop);
  ~Channel();

  int GetFd();
  uint32_t GetEvents();
  uint32_t GetTheMomentEvents();
  void SetTheMomentEvents(uint32_t the_moment_events);
  bool GetIsInEpoll();
  void SetIsInEpoll(bool is_in_epoll);
  void EnableRead();
  void UseET();

  void HandleEvent();
  void SetReadCallBack(std::function<void()> &&read_call_back);
  void SetWriteCallBack(std::function<void()> &&write_call_back);
  void SetUseThreadPool(bool use_thread_pool);

 private:
  int fd_;           // 对应的文件描述符
  EventLoop *loop_;  // 对应的EventLoop
  uint32_t events_;  // 要监听的事件类型
  uint32_t the_moment_events_;  // 在该fd上正在发生的事件类型，也就是调用epoll_wait时该fd上发生的事件类型
  bool is_in_epoll_;                       // 该fd是否加入了epoll的事件表
  bool use_thread_pool_;                   // 是否使用线程池运行回调函数
  std::function<void()> read_call_back_;   // 回调函数，用于处理读事件
  std::function<void()> write_call_back_;  // 回调函数，处理写事件
};

#endif  // SRC_INCLUDE_CHANNEL_H_
