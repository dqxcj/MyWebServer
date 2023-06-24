#include "Acceptor.h"
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "include/Acceptor.h"
#include <iostream>

Acceptor::Acceptor(std::shared_ptr<EventLoop> loop, const std::string &serv_ip, uint16_t serv_port) : 
  loop_(loop),
  serv_sock_(std::make_shared<Socket>()),
  acceptor_channel_(std::make_shared<Channel>(serv_sock_->GetFd(), loop_))  {

  auto serv_addr = std::make_shared<InetAddress>(serv_ip, serv_port);
  serv_sock_->Bind(serv_addr);
  serv_sock_->Listen();
  // serv_sock_->SetNonBlock();

  std::function<void()> call_back = std::bind(&Acceptor::HandleNewConnection, this);
  acceptor_channel_->SetReadCallBack(std::move(call_back));
  acceptor_channel_->EnableRead();
  acceptor_channel_->SetUseThreadPool(false);
}

Acceptor::~Acceptor() {
  // delete serv_sock_;
}

void Acceptor::HandleNewConnection() {
  auto clnt_addr = std::make_shared<InetAddress>();
  auto clnt_sock = std::make_shared<Socket>(serv_sock_->Accept(clnt_addr));
  // std::cout << "clnt_fd: " << clnt_sock->GetFd() << std::endl;
  new_connection_call_back_(clnt_sock);
}

void Acceptor::SetNewConnectionCallBack(std::function<void(std::shared_ptr<Socket>)> &&new_connection_call_back) {
  new_connection_call_back_ = new_connection_call_back;
}

std::shared_ptr<Socket> Acceptor::GetServSocket() {
  return serv_sock_;
}
