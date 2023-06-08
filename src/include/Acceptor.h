#ifndef SRC_INCLUDE_ACCEPTOR_H_
#define SRC_INCLUDE_ACCEPTOR_H_
#include <functional>
#include <memory>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
 public:
  explicit Acceptor(EventLoop *loop);
  ~Acceptor();

  void HandleNewConnection();
  void SetNewConnectionCallBack(std::function<void(Socket *)> &&new_connection_call_back);
  
  Socket *GetServSocket();
 private:
  EventLoop *loop_;
  Socket *serv_sock_;
  Channel *acceptor_channel_;
  std::function<void(Socket *)> new_connection_call_back_;
};

#endif  // SRC_INCLUDE_ACCEPTOR_H_
