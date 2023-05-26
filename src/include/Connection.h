#ifndef SRC_INCLUDE_CONNECTION_H_
#define SRC_INCLUDE_CONNECTION_H_
#include <unistd.h>

#include <functional>
#include <iostream>

class Socket;
class Channel;
class EventLoop;
class Buffer;

class Connection {
 public:
  Connection() = default;
  Connection(EventLoop *loop_, Socket *clnt);
  ~Connection();
  void Echo(int rw_fd);
  void SetDeleteConnectionCallBack(std::function<void(Socket *)> delete_connection_call_back);

 private:
  EventLoop *loop_;
  Channel *channel_;
  Socket *clnt_;
  Buffer *read_buffer_;
  std::function<void(Socket *)> delete_connection_call_back_;
};

#endif  // SRC_INCLUDE_CONNECTION_H_
