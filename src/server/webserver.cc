#include "webserver.h"
using namespace std;

WebServer::WebServer(int port, int time_out_ms) : port_(port), time_out_ms_(time_out_ms), epoller_(new Epoller()) {
    bool res = InitSocket_();
    assert(res);
}

WebServer::~WebServer() {
    close(listen_fd_);
}

bool WebServer::InitSocket_() {
    // 创建socket
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd_ == -1) {
        perror("socket");
        return false;
    }

    // 设置端口复用
    int optval = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 设置sockaddr
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    // 为什么设置为INADDR_ANY?
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port_);

    // 绑定
    int ret = bind(listen_fd_, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        return false;
    }

    // 监听
    ret = listen(listen_fd_, 128);
    if(ret == -1) {
        perror("listen");
        return false;
    }

    // 将listen_fd_注册道epoll内核事件表中
    epoller_->AddFd(listen_fd_, EPOLLIN);

    cout << "server is ok" << endl;

    // 循环处理每一批就绪事件
    while(true) {
        // 就绪事件数量
        int num = epoller_->Wait(time_out_ms_);
        if(num == -1) {
            perror("epoll_wait");
            exit(-1);
        }
        cout << "num = " << num << endl;

        // 处理这一批就绪事件
        for(int i = 0; i < num; i++) {
            int curfd = epoller_->GetEventFd(i);

            // 有客户端想要连接服务端
            if(curfd == listen_fd_) {
                // 接受客户连接
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int client_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &len);
                if (client_fd == -1) {
                    perror("accept");
                    return false;
                }

                // 获取客户端信息
                char client_ip[16];
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
                unsigned short client_port = ntohs(client_addr.sin_port);

                // 输出客户端的信息
                cout << "client's ip is " << client_ip << ", and port is " << client_port << endl;

                // 设置文件描述符为非阻塞
                int flag = fcntl(client_fd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(client_fd, F_SETFL, flag);

                // 将client_fd注册到内核事件表中，设置为ET模式
                epoller_->AddFd(client_fd, EPOLLIN | EPOLLET);
            } else {  // 已连接的客户端发送了消息给服务端
                // 这里不太理解为什么要跳过写事件
                // 猜测可能是后续会向内核事件表注册写事件
                // 所以这里过滤一下
                if(epoller_->GetEvents(i) & EPOLLOUT) {
                    continue;
                }
                
                // 接受数据并处理
                // 为什么设置为5，而不是1或者一个较大值？
                char buf[5];
                int len = 0;
                // 这里使用了recv + sent，若想要更强的通用性，可以使用read + write
                while((len = recv(curfd, buf, sizeof(buf), 0)) > 0) {
                    // 将接收到的数据输出到标准输出中
                    int tmp = write(STDOUT_FILENO, buf, len);
                    if(tmp < 0) {
                        perror("write");
                        return false;
                    }
                    // 转换成大写
                    for (int j = 0; j < len; j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    // 发送数据给客户端
                    int ret = send(curfd, buf, sizeof(buf), 0);
                    if(ret == -1) {
                        perror("send");
                        return false;
                    }
                }
                if(len == 0) {
                    cout << "client close..." << endl;
                    epoller_->DelFd(curfd);
                    close(curfd);
                } else if(len == -1) {
                    // 非阻塞I/O这个标志说明事件未发生，也就是读完了
                    if(errno == EAGAIN) {
                        cout << "recv over ..." << endl;
                    } else {
                        perror("read");
                        exit(-1);
                    }
                }
            }
        }
    }
    return true;
}   