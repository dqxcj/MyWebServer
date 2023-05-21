#ifndef INETADDRESS_H
#define INETADDRESS_H
#include <arpa/inet.h>

class InetAddress {
public:
    sockaddr_in addr;
    socklen_t addr_len;
    InetAddress();
};

#endif