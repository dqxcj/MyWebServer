#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "MyError.h"
#include <string>
using namespace std;

int main() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  ErrIf(sockfd == -1, "client.cpp error: socket()");
  struct sockaddr_in serv_addr;
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(80);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  ErrIf(connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "client error: accept()");

  string str = "GET / HTTP/1.1\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\nCache-Control: no-cache\r\nConnection: keep-alive";

  if (true) {
    int sz = str.size();
    char *buf = const_cast<char *>(str.c_str());
    // bzero(buf, sizeof(buf));
    // std::cin >> buf;
    ssize_t write_len = write(sockfd, buf, sz);
    ErrIf(write_len == -1, "client error: write to server");
    // std::cout << "client send " << write_len << " bytes to server " << inet_ntoa(serv_addr.sin_addr) << std::endl;

    bzero(buf, sz);
    ssize_t read_len = read(sockfd, buf, sz);
    if (read_len > 0) {
      // std::cout << read_len << " bytes message from server fd " << sockfd << ": " << buf << std::endl;
    } else if (read_len == 0) {
      // std::cout << "server fd " << sockfd << " disconnected" << std::endl;
      // break;
    } else if (read_len == -1) {
      close(sockfd);
      ErrIf(true, "client error: read from server");
    }
  }

  close(sockfd);
  return 0;
}
