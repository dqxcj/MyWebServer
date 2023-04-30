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
// cpp库
#include <memory>
// 其它项目的h文件
// 本项目的h文件
#include "include/epoller.h"

class WebServer {
public:
    WebServer(int);
private:
    bool InitSocket_();
    int time_out_ms_;
    int port_;
    int listen_fd_;
    std::unique_ptr<Epoller> epoller_;
};
#endif