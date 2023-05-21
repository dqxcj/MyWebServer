#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "MyError.h"
using namespace std;

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(sockfd == -1, "server error: socket()");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);
    ErrIf(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "server error: bind()");

    ErrIf(listen(sockfd, SOMAXCONN) == -1, "server error: listen()");

    int epfd = epoll_create1(0);
    ErrIf(epfd == -1, "server error: epoll_create1()");

    struct epoll_event events[MAX_EVENTS], ev;
    memset(events, 0, sizeof(events));
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET; // ET模式
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK); // 设置成非阻塞
    ErrIf(epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "server error: epoll_ctl()");
    
    while(true) {
        int ev_num = epoll_wait(epfd, events, MAX_EVENTS, -1);
        ErrIf(ev_num == -1, "server error: epoll_wait()");
        for(int i = 0; i < ev_num; i++) {
            if(events[i].data.fd == sockfd) { // 新客户端连接
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                memset(&clnt_addr, 0, sizeof(clnt_addr));
                int clnt_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len);
                ErrIf(clnt_sockfd == -1, "server error: accept()");
                cout << "new client fd " << clnt_sockfd << "! Ip: " << inet_ntoa(clnt_addr.sin_addr) << " Port: " << ntohs(clnt_addr.sin_port) << endl;
            
                memset(&ev, 0, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET; // ET模式
                fcntl(clnt_sockfd, F_SETFL, fcntl(clnt_sockfd, F_GETFL) | O_NONBLOCK); // 设置成非阻塞
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if(events[i].events & EPOLLIN) { // 可读事件
                char buf[READ_BUFFER];
                int rw_fd = events[i].data.fd;
                while(true) { // 非阻塞IO，所以循环读取以读取全部数据
                    memset(buf, 0, sizeof(buf));
                    ssize_t read_len = read(rw_fd, buf, sizeof(buf));
                    if(read_len > 0) {
                        cout << read_len << " bytes message from client fd "<< rw_fd << ": " << buf << endl;
                        ssize_t write_len = write(rw_fd, buf, read_len);
                        ErrIf( write_len == -1, "server error: write to client");
                        cout << "server send " << write_len << " bytes to client fd " << rw_fd << endl;
                    } else if(read_len == 0) { // EOF 客户端断开连接
                        cout << "client fd " << rw_fd << " disconnected" << endl;
                        close(rw_fd);
                        break;
                    } else if(read_len == -1 && errno == EINTR) { // 正常中断，接着读取
                        cout << "client fd " << rw_fd << " will continue reading" << endl;
                        continue;
                    } else if(read_len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // 非阻塞IO，数据读取完毕
                        cout << "finsh reading once, errno: " << errno << endl;
                        break; 
                    }
                }   
            } else { // 之后可能会实现其它事件
                cout << "something else happend" << endl;
            }
        }
    }

    close(sockfd);
    return 0; 
}