#include "Acceptor.h"
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "include/Acceptor.h"

Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
  serv_sock_ = new Socket();
  InetAddress *serv_addr_ = new InetAddress("127.0.0.1", 6666);
  serv_sock_->Bind(serv_addr_);
  serv_sock_->Listen();
  // serv_sock_->SetNonBlock();

  acceptor_channel_ = new Channel(serv_sock_->GetFd(), loop_);
  std::function<void()> call_back = std::bind(&Acceptor::HandleNewConnection, this);
  acceptor_channel_->SetReadCallBack(std::move(call_back));
  acceptor_channel_->EnableRead();
  acceptor_channel_->SetUseThreadPool(false);
  delete serv_addr_;
}

Acceptor::~Acceptor() {
  delete serv_sock_;
  delete acceptor_channel_;
}

void Acceptor::HandleNewConnection() {
  auto clnt_addr = std::make_shared<InetAddress>();
  Socket *clnt_sock = new Socket(serv_sock_->Accept(clnt_addr));
  new_connection_call_back_(clnt_sock);
  delete clnt_sock;
}

void Acceptor::SetNewConnectionCallBack(std::function<void(Socket *)> &&new_connection_call_back) {
  new_connection_call_back_ = new_connection_call_back;
}
