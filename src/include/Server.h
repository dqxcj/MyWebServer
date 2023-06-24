#ifndef SRC_INCLUDE_SERVER_H_
#define SRC_INCLUDE_SERVER_H_

#include <unistd.h>

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>


class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
 public:
  explicit Server(std::shared_ptr<EventLoop> loop, const std::string &serv_ip = "127.0.0.1", uint16_t serv_port = 55555);
  ~Server();

  void NewConnection(std::shared_ptr<Socket> clnt);
  void DeleteConnection(std::shared_ptr<Socket> clnt);
  void HandleReadEvent(int rw_fd);

 private:
  std::shared_ptr<EventLoop> main_reactor_;                // 主reactor 负责处理连接
  std::shared_ptr<Acceptor> acceptor_;                                     // 连接器
  std::unordered_map<int, std::shared_ptr<Connection>> tcp_connections_;  // 所有tcp连接
  std::vector<std::shared_ptr<EventLoop>> sub_reactors_;                  // 副reactor 负责业务逻辑
  std::shared_ptr<ThreadPool> thread_pool_;                // 线程池
};

#endif  // SRC_INCLUDE_SERVER_H_
