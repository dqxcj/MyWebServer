#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    // 创建socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    // 设置 socket addr
    struct sockaddr_in server_addr;
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    // 连接服务器
    int ret = connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }

    char send_buf[1024] = {0};
    while (true) {
        fgets(send_buf, sizeof(send_buf), stdin);
        write(fd, send_buf, strlen(send_buf) + 1);
        int len = read(fd, send_buf, sizeof(send_buf));
        if (len == -1) {
            perror("read");
            return -1;
        } else if (len > 0) {
            printf("read buf = %s\n", send_buf);
        } else {
            printf("server disconnect...\n");
            break;
        }
    }

    close(fd);
    return 0;
}