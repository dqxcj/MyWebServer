#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

const int READ_BUFFER = 1024;

Connection::Connection(EventLoop *loop, Socket *clnt): loop_(loop), clnt_(clnt) {
    channel_ = new Channel(clnt_->GetFd(), loop_);
    channel_->EnableRead();
    clnt_->SetNonBlock();
    channel_->UseET();
    std::function<void()> call_back = std::bind(&Connection::Echo, this, clnt->GetFd());
    channel_->SetReadCallBack(std::move(call_back));
    channel_->SetUseThreadPool(true);
    read_buffer_ = new Buffer();
}
Connection::~Connection() {
    delete channel_;
    delete read_buffer_;
}
void Connection::Echo(int rw_fd) {
    char buf[READ_BUFFER];
    while(true) { // 非阻塞IO，所以循环读取以读取全部数据
        memset(buf, 0, sizeof(buf));
        ssize_t read_len = read(rw_fd, buf, sizeof(buf));
        read_buffer_->Write(buf, read_len);
        if(read_len > 0) {
            std::cout << read_len << " bytes message from client fd "<< rw_fd << ": " << buf << std::endl;
            ssize_t write_len = write(rw_fd, read_buffer_->CStr(), read_len);
            read_buffer_->Clear();
            ErrIf( write_len == -1, "server error: write to client");
            std::cout << "server send " << write_len << " bytes to client fd " << rw_fd << std::endl;
        } else if(read_len == 0) { // EOF 客户端断开连接
            std::cout << "client fd " << rw_fd << " disconnected" << std::endl;
            delete_connection_call_back_(clnt_);
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
void Connection::SetDeleteConnectionCallBack(std::function<void(Socket *)> delete_connection_call_back) {
    delete_connection_call_back_ = delete_connection_call_back;
}