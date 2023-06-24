#ifndef SRC_INCLUDE_SOCKET_H_
#define SRC_INCLUDE_SOCKET_H_
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <memory>

class InetAddress;

class Socket {
 public:
  Socket();

  explicit Socket(int fd);

  void Bind(const std::shared_ptr<InetAddress> &addr);

  void Listen();

  int Accept(std::shared_ptr<InetAddress> clnt_addr);

  int GetFd();

  void SetNonBlock();

  void Connect(std::shared_ptr<InetAddress> serv_addr);

 private:
  int fd_;  // 该socket的文件描述符
};

#endif  // SRC_INCLUDE_SOCKET_H_
