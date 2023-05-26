#ifndef SRC_INCLUDE_SOCKET_H_
#define SRC_INCLUDE_SOCKET_H_
#include "InetAddress.h"
#include "MyError.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

class Socket {
 public:
  Socket() = default;

  explicit Socket(int fd);

  void Bind(InetAddress *addr);

  void Listen();

  int Accept(InetAddress *clnt_addr);

  int GetFd();

  void SetNonBlock();

  void Connect(InetAddress *serv_addr);

 private:
  int fd_;  // 该socket的文件描述符
};

#endif  // SRC_INCLUDE_SOCKET_H_
