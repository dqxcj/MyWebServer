#include "Server.h"
#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "log.h"

Server::Server(EventLoop *loop) : main_reactor_(loop) {
  // 将main_reactor绑定到acceptor_上，专门处理tcp连接
  acceptor_ = new Acceptor(main_reactor_);
  std::function<void(Socket *)> call_back = std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallBack(std::move(call_back));

  // 获取最大线程数，一般指处理器数量
  int cnt = std::thread::hardware_concurrency();

  // 生成sub_reactors_
  for (int i = 0; i < cnt; i++) {
    sub_reactors_.emplace_back(new EventLoop());
  }

  // 构造线程池
  thpoll = new ThreadPool(cnt);

  // 将每个sub_reactor与一个线程绑定
  for (int i = 0; i < cnt; i++) {
    std::function<void()> func = std::bind(&EventLoop::Loop, sub_reactors_[i]);
    thpoll->AddTask(std::move(func));
  }

  Log::Instance()->init(1, "./log", ".log", 1024);
  LOG_INFO("========== Server init==========");
  LOG_INFO("thread num: %d", cnt);
}

Server::~Server() {
  delete acceptor_;
  for (auto &loop : sub_reactors_) {
    delete loop;
  }
  delete thpoll;
}

void Server::NewConnection(Socket *clnt) {
  // 将新连接随机绑定到一个sub_reactor上
  int random = clnt->GetFd() % sub_reactors_.size();
  Connection *connection = new Connection(sub_reactors_[random], clnt);
  std::function<void(Socket *)> call_back = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
  connection->SetDeleteConnectionCallBack(call_back);
  tcp_connections_[clnt->GetFd()] = connection;
}

void Server::DeleteConnection(Socket *clnt) {
  Connection *tmp = tcp_connections_[clnt->GetFd()];
  tcp_connections_.erase(clnt->GetFd());
  delete tmp;
}

void Server::HandleReadEvent(int rw_fd) { ; }
