#ifndef SRC_INCLUDE_ACCEPTOR_H_
#define SRC_INCLUDE_ACCEPTOR_H_
#include <functional>
#include <memory>
#include "Macros.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
 public:
  explicit Acceptor(std::shared_ptr<EventLoop> loop, const std::string &serv_ip, uint16_t serv_port);
  ~Acceptor();

  DELETE_COPY_AND_MOVE(Acceptor)

  void HandleNewConnection();
  void SetNewConnectionCallBack(std::function<void(std::shared_ptr<Socket>)> &&new_connection_call_back);

  std::shared_ptr<Socket> GetServSocket();
 private:
  std::shared_ptr<EventLoop> loop_;
  std::shared_ptr<Socket> serv_sock_;
  std::shared_ptr<Channel> acceptor_channel_;
  std::function<void(std::shared_ptr<Socket>)> new_connection_call_back_;
};

#endif  // SRC_INCLUDE_ACCEPTOR_H_
