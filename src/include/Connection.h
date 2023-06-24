#ifndef SRC_INCLUDE_CONNECTION_H_
#define SRC_INCLUDE_CONNECTION_H_
#include <unistd.h>

#include <functional>
#include <iostream>
#include <memory>

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
  Connection(std::shared_ptr<EventLoop> loop, std::shared_ptr<Socket> clnt);
  ~Connection();
  // void Echo(int rw_fd);

  // 设置 连接删除 回调函数
  void SetDeleteConnectionCallBack(std::function<void(std::shared_ptr<Socket>)> delete_connection_call_back);

  // http业务逻辑
  void HttpServer(std::shared_ptr<Socket> clnt);

 private:
  std::shared_ptr<EventLoop> loop_;
  std::shared_ptr<Channel> channel_;
  std::shared_ptr<Socket> clnt_;
  // Buffer *read_buffer_{nullptr};
  // Buffer *write_buffer_{nullptr};
  State state_{State::Invalid};
  std::function<void(std::shared_ptr<Socket>)> delete_connection_call_back_;
  std::function<void(Connection *)> on_connect_call_back_;
};

#endif  // SRC_INCLUDE_CONNECTION_H_
