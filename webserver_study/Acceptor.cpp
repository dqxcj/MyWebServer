#include "Acceptor.h"
#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop *loop): loop_(loop) {
    serv_sock_ = new Socket();                                
    serv_addr_ = new InetAddress("127.0.0.1", 8889);
    serv_sock_->Bind(serv_addr_);
    serv_sock_->Listen();
    serv_sock_->SetNonBlock();

    acceptor_channel_ = new Channel(serv_sock_->GetFd(), loop_);
    std::function<void()> call_back = std::bind(&Acceptor::HandleNewConnection, this);
    acceptor_channel_->SetCallBack(std::move(call_back));
    acceptor_channel_->EnableRead();
}

Acceptor::~Acceptor() {
    delete serv_sock_;
    delete serv_addr_;
    delete acceptor_channel_;
}

void Acceptor::HandleNewConnection() {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock_->Accept(clnt_addr));
    new_connection_call_back_(clnt_sock);
    delete clnt_addr;
    delete clnt_sock;
}

void Acceptor::SetNewConnectionCallBack(std::function<void(Socket *)> &&new_connection_call_back) {
    new_connection_call_back_ = new_connection_call_back;
}