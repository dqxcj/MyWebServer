#include "Server.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"

Server::Server(EventLoop *loop): loop_(loop),acceptor_(nullptr) {
    acceptor_ = new Acceptor(loop_);
    std::function<void(Socket *)> call_back = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor_->SetNewConnectionCallBack(std::move(call_back));
}

Server::~Server() {
    delete acceptor_;
}

void Server::NewConnection(Socket *clnt) {
    Connection *connection = new Connection(loop_, clnt);
    std::function<void(Socket*)> call_back = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    connection->SetDeleteConnectionCallBack(call_back);
    tcp_connections_[clnt->GetFd()] = connection;
}

void Server::DeleteConnection(Socket *clnt) {
    Connection *tmp = tcp_connections_[clnt->GetFd()];
    tcp_connections_.erase(clnt->GetFd());
    delete tmp;
}

void Server::HandleReadEvent(int rw_fd) {
    ;
}