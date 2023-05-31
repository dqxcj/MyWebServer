#ifndef SRC_INCLUDE_CONNECTION_H_
#define SRC_INCLUDE_CONNECTION_H_
#include <unistd.h>

#include <functional>
#include <iostream>

class Socket;
class Channel;
class EventLoop;
// class Buffer;

class Connection {
 public:
  enum State {
    Invalid = 1, // 非法
    Handshaking, // 握手中
    Connected,   // 已连接
    Closed,      // 已关闭
    Failed,      // 失败
  };
  Connection(EventLoop *loop_, Socket *clnt);
  ~Connection();
  // void Echo(int rw_fd);
  void SetDeleteConnectionCallBack(std::function<void(Socket *)> delete_connection_call_back);
  
  // http业务逻辑
  void HttpServer(Socket *clnt);

 private:
  EventLoop *loop_;
  Channel *channel_{nullptr};
  Socket *clnt_;
  // Buffer *read_buffer_{nullptr};
  // Buffer *write_buffer_{nullptr};
  State state_{State::Invalid};
  std::function<void(Socket *)> delete_connection_call_back_;
  std::function<void(Connection *)> on_connect_call_back_;
};

#endif  // SRC_INCLUDE_CONNECTION_H_
