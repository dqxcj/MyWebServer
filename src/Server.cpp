#include "Server.h"
#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "Log.h"

#include <unistd.h>
#include <string.h>
#include <memory>

Server::Server(std::shared_ptr<EventLoop> loop, const std::string &serv_ip, uint16_t serv_port) : main_reactor_(std::move(loop)) {
  // 将main_reactor绑定到acceptor_上，专门处理tcp连接
  acceptor_ = std::make_shared<Acceptor>(main_reactor_, serv_ip, serv_port);
  std::function<void(std::shared_ptr<Socket>)> call_back = std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallBack(std::move(call_back));

  // 获取最大线程数，一般指处理器数量
  int cnt = std::thread::hardware_concurrency();

  // 生成sub_reactors_
  for (int i = 0; i < cnt; i++) {
    sub_reactors_.emplace_back(std::make_shared<EventLoop>());
  }

  // 构造线程池
  thread_pool_ = std::make_shared<ThreadPool>(cnt);

  // 将每个sub_reactor与一个线程绑定
  for (int i = 0; i < cnt; i++) {
    std::function<void()> func = std::bind(&EventLoop::Loop, sub_reactors_[i]);
    thread_pool_->AddTask(std::move(func));
  }

  char pwd_path[128];
  getcwd(pwd_path, sizeof(pwd_path));
  const char *log_path = strcat(pwd_path, "/log");
  Log::Instance()->init(1, log_path, ".log", 1024);
  LOG_INFO("========== Server init==========");
  LOG_INFO("thread num: %d", cnt);
  LOG_INFO("pwd: %s", pwd_path);
  LOG_INFO("log path: %s", log_path);
}

Server::~Server() {

}

void Server::NewConnection(std::shared_ptr<Socket> clnt) {
  // 将新连接随机绑定到一个sub_reactor上
  int random = clnt->GetFd() % sub_reactors_.size();
  auto connection = std::make_shared<Connection>(sub_reactors_[random], clnt);
  std::function<void(std::shared_ptr<Socket>)> call_back = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
  connection->SetDeleteConnectionCallBack(call_back);
  tcp_connections_[clnt->GetFd()] = connection;
}

void Server::DeleteConnection(std::shared_ptr<Socket> clnt) {
  tcp_connections_.erase(clnt->GetFd());
}

void Server::HandleReadEvent(int rw_fd) { ; }
