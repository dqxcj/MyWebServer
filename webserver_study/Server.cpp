#include "Server.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

Server::Server(EventLoop *loop): loop_(loop) {
    Socket *sock = new Socket();                                 // 此处内存未释放
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8889); // 此处内存未释放
    sock->Bind(serv_addr);
    sock->Listen();
    sock->SetNonBlock();

    Channel *channel = new Channel(sock->GetFd(), loop_); // 此处内存未释放
    std::function<void()> call_back = std::bind(&Server::NewConnection, this, sock);
    channel->SetCallBack(std::move(call_back));
    channel->EnableRead();
}

void Server::NewConnection(Socket *serv) {
    InetAddress *clnt_addr = new InetAddress(); // 此处未释放内存
    Socket *clnt = new Socket(serv->Accept(clnt_addr));

    clnt->SetNonBlock();
    Channel *channel = new Channel(clnt->GetFd(), loop_); // 此处未释放内存
    std::function<void()> call_back = std::bind(&Server::HandleReadEvent, this, clnt->GetFd());
    channel->SetCallBack(std::move(call_back));
    channel->EnableRead();
}

void Server::HandleReadEvent(int rw_fd) {
    char buf[READ_BUFFER];
    while(true) { // 非阻塞IO，所以循环读取以读取全部数据
        memset(buf, 0, sizeof(buf));
        ssize_t read_len = read(rw_fd, buf, sizeof(buf));
        if(read_len > 0) {
            std::cout << read_len << " bytes message from client fd "<< rw_fd << ": " << buf << std::endl;
            ssize_t write_len = write(rw_fd, buf, read_len);
            ErrIf( write_len == -1, "server error: write to client");
            std::cout << "server send " << write_len << " bytes to client fd " << rw_fd << std::endl;
        } else if(read_len == 0) { // EOF 客户端断开连接
            std::cout << "client fd " << rw_fd << " disconnected" << std::endl;
            close(rw_fd);
            break;
        } else if(read_len == -1 && errno == EINTR) { // 正常中断，接着读取
            std::cout << "client fd " << rw_fd << " will continue reading" << std::endl;
            continue;
        } else if(read_len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // 非阻塞IO，数据读取完毕
            std::cout << "finsh reading once, errno: " << errno << std::endl;
            break; 
        }
    }   
}