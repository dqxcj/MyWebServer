#include "Socket.h"
#include <iostream>

Socket::Socket() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(fd_ == -1, "server error: socket()");
}

void Socket::Bind(InetAddress *addr) {
    ErrIf(bind(fd_, (sockaddr *)&addr->addr_, addr->addr_len_) == -1, "server error: bind()");
}

void Socket::Listen() {
    ErrIf(listen(fd_, SOMAXCONN) == -1, "server error: listen()");
}

int Socket::Accept(InetAddress *clnt_addr) {
    int clnt_sockfd = accept(fd_, (sockaddr *)&(clnt_addr->addr_), &(clnt_addr->addr_len_));
    ErrIf(clnt_sockfd == -1, "server error: accept()");
    std::cout << "new client fd " << clnt_sockfd << "! Ip: " << inet_ntoa(clnt_addr->addr_.sin_addr) << " Port: " << ntohs(clnt_addr->addr_.sin_port) << std::endl;
    return clnt_sockfd;
}

int Socket::GetFd() {
    return fd_;
}