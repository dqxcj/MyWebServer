#include "MyError.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"
#include "Channel.h"

#include <iostream>
#include <vector>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

#define READ_BUFFER 1024

void HandRead(int rw_fd);

int main() {
    Socket *sock = new Socket();  // 此处内存未释放
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8889); // 此处内存未释放
    sock->Bind(serv_addr);
    sock->Listen();

    Epoll *epoll = new Epoll(0, 1024); // 此处内存未释放

    Channel *channel = new Channel(sock->GetFd(), epoll); // 此处内存未释放
    channel->EnableRead();

    while(true) {
        vector<Channel *> channels = epoll->Wait(-1);
        for(auto &channel : channels) {
            if(channel->GetFd() == sock->GetFd()) { // 新客户端连接
                InetAddress *clnt_addr = new InetAddress(); // 此处未释放内存
                int clnt_sockfd = sock->Accept(clnt_addr);

                Channel *channel = new Channel(clnt_sockfd, epoll); // 此处未释放内存

                channel->EnableRead();

            } else if(channel->GetTheMomentEvents() & EPOLLIN) { // 可读事件
                HandRead(channel->GetFd());

            } else { // 之后可能会实现其它事件
                cout << "something else happend" << endl;
            }
        }
    }

    close(sock->GetFd());
    return 0; 
}

void HandRead(int rw_fd) {
    char buf[READ_BUFFER];
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
}