#ifndef SOCKET_H
#define SOCKET_H
#include "MyError.h"
#include "InetAddress.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Socket {
public:
    Socket();

    Socket(int fd);

    void Bind(InetAddress *addr);

    void Listen();

    int Accept(InetAddress *clnt_addr);

    int GetFd();

    void SetNonBlock();

    void Connect(InetAddress *serv_addr);
private:
    int fd_; // 该socket的文件描述符
};

#endif