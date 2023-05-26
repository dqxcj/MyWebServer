#ifndef INETADDRESS_H
#define INETADDRESS_H
#include <arpa/inet.h>
#include <string.h>

class InetAddress {
public:
    sockaddr_in addr_;
    socklen_t addr_len_;
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
};

#endif