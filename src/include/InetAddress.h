#ifndef SRC_INCLUDE_INETADDRESS_H_
#define SRC_INCLUDE_INETADDRESS_H_
#include <arpa/inet.h>
#include <string.h>

class InetAddress {
 public:
  sockaddr_in addr_;
  socklen_t addr_len_;
  InetAddress();
  InetAddress(const char *ip, uint16_t port);
};

#endif  // SRC_INCLUDE_INETADDRESS_H_
