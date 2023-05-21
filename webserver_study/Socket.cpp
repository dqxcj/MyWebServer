#include "Socket.h"

Socket::Socket() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(fd_ == -1, "server error: socket()");
}

void Socket::Bind(InetAddress *addr) {
    ErrIf(bind(fd_, (sockaddr *)&addr->addr, addr->addr_len) == -1, "server error: bind()");
}

