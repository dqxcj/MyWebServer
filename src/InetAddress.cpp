#include "InetAddress.h"
#include <cstring>

InetAddress::InetAddress(): addr_({0}), addr_len_(sizeof(addr_)) {
  memset(&addr_, 0, sizeof(addr_));
}

InetAddress::InetAddress(const std::string &addr_ip, uint16_t addr_port) : addr_({0}), addr_len_(sizeof(addr_)) {
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(addr_ip.data());
  addr_.sin_port = htons(addr_port);
}
