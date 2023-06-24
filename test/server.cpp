#include "Server.h"
#include "EventLoop.h"
#include <string>
#include <iostream>
#include <memory>

int main(int argc, char **argv) {
  std::string serv_ip;
  uint16_t serv_port = 0;
  if(argc > 1) {
    serv_ip = argv[1];
    serv_port = std::stoi(argv[2]);
  }
  // std::cout << "srever ip: " << serv_ip << " server port: " << serv_port << std::endl;
  // EventLoop *loop = new EventLoop();
  auto loop = std::make_shared<EventLoop>();
  // Server *server = new Server(loop, serv_ip, serv_port);
  auto server = std::make_shared<Server>(loop, serv_ip, serv_port);
  loop->Loop();
  return 0;
}
