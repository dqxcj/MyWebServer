#ifndef WEBSERVER_H
#define WEBSERVER_H

// 相关文件
// c库
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
// cpp库
#include <iostream>
#include <memory>
// 其它项目的h文件
// 本项目的h文件
#include "epoller.h"

class WebServer {
public:
    WebServer(int port, int time_out_ms);
    ~WebServer();
private:
    bool InitSocket_();
    int time_out_ms_; // epoll_wait的超时时间
    int port_; // 服务端监听的端口
    int listen_fd_; // 服务端监听的文件描述符
    std::unique_ptr<Epoller> epoller_; // 指向了一个Epoller类
};
#endif