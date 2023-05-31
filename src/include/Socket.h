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

  void Bind(InetAddress *addr);

  void Listen();

  int Accept(std::shared_ptr<InetAddress> clnt_addr);

  int GetFd();

  void SetNonBlock();

  void Connect(InetAddress *serv_addr);

  std::shared_ptr<InetAddress> GetClntAddr();

 private:
  int fd_;  // 该socket的文件描述符
  std::shared_ptr<InetAddress> clnt_addr_{nullptr};
};

#endif  // SRC_INCLUDE_SOCKET_H_
