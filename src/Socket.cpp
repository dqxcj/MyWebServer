#include "Socket.h"
#include "InetAddress.h"
#include "MyError.h"

#include <iostream>
#include <memory>

Socket::Socket() {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  ErrIf(fd_ == -1, "server error: socket()");
}

Socket::Socket(int fd) : fd_(fd) {}

void Socket::Bind(const std::shared_ptr<InetAddress> &addr) {
  ErrIf(bind(fd_, (sockaddr *)&addr->addr_, addr->addr_len_) == -1, "server error: bind()");
}

void Socket::Listen() { ErrIf(listen(fd_, SOMAXCONN) == -1, "server error: listen()"); }

int Socket::Accept(std::shared_ptr<InetAddress> clnt_addr) {
  int clnt_sockfd = accept(fd_, (sockaddr *)&(clnt_addr->addr_), &(clnt_addr->addr_len_));
  ErrIf(clnt_sockfd == -1, "server error: accept()");
  // std::cout << "new client fd " << clnt_sockfd << "! Ip: " << inet_ntoa(clnt_addr->addr_.sin_addr)
            // << " Port: " << ntohs(clnt_addr->addr_.sin_port) << std::endl;
  return clnt_sockfd;
}

int Socket::GetFd() { return fd_; }

void Socket::SetNonBlock() {
  fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);  // 设置成非阻塞
}

void Socket::Connect(std::shared_ptr<InetAddress> serv_addr) {
  ErrIf(connect(fd_, (sockaddr *)&serv_addr->addr_, serv_addr->addr_len_) == -1, "client error: accept()");
}
