#ifndef SRC_INCLUDE_EPOLL_H_
#define SRC_INCLUDE_EPOLL_H_
#include "Channel.h"
#include "MyError.h"

#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>

#include <vector>

const int MAX_EVENTS_NUM = 1024;

class Epoll {
 public:
  Epoll();
  explicit Epoll(int flags, int max_events_num = MAX_EVENTS_NUM);

  void AddFd(int fd, uint32_t events, bool is_no_block);

  // std::vector<epoll_event> Wait(int timeout);
  std::vector<Channel *> Wait(int timeout);

  void UpdateChannel(Channel *channel);

 private:
  int epfd_;
  int max_events_num_;
  std::vector<epoll_event> events_;
};

#endif  // SRC_INCLUDE_EPOLL_H_
