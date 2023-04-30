#include "include/webserver.h"

WebServer::WebServer(int n) {
    InitSocket_();
}

bool WebServer::InitSocket_() {
    // 创建socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1) {
        perror("socket");
        return -1;
    }

    // 设置端口复用
    int optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 设置sockaddr
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    // 为什么设置为INADDR_ANY?
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(9999);

    // 绑定
    int ret = bind(listen_fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        return -1;
    }

    // 监听
    ret = listen(listen_fd, 128);
    if(ret == -1) {
        perror("listen");
        return -1;
    }
    // 接受客户连接
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
    if(client_fd == -1) {
        perror("accept");
        return -1;
    }

    // 获取客户端信息
    char client_ip[16];
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    unsigned short client_port = ntohs(client_addr.sin_port);

    // 输出客户端的信息
    printf("client's ip is %s, and port is %d\n", client_ip, client_port);

    // 接受客户端发来的数据
    char recv_buf[1024] = {0};
    while(true) {
        int len = recv(client_fd, recv_buf, sizeof(recv_buf), 0);
        if(len == -1) {
            perror("recv");
            return -1;
        } else if(len == 0) { // 对方已关闭连接
            printf("client disconnect...\n");
            break;
        } else {
            printf("read buf = %s\n", recv_buf);
        }
        // 转换成大写
        for(int i = 0; i < len; i++) {
            recv_buf[i] = toupper(recv_buf[i]);
        }
        ret = send(client_fd, recv_buf, strlen(recv_buf) + 1, 0);
        if(ret == -1) {
            perror("send");
            return -1;
        }
    }

    close(client_fd);
    close(listen_fd);
    return 0;
}   