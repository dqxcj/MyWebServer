#include "HttpConn.h"

std::atomic<int> HttpConn::user_cnt_ = {0};

HttpConn::HttpConn() {
    fd_ = -1;
    addr_ = {0};
    is_close_ = true;
}
HttpConn::~HttpConn() {
    Close();
}

void HttpConn::Open(int fd, sockaddr_in addr) {
    assert(fd >= 0);
    fd_ = fd;
    addr_ = addr;
    user_cnt_++;
    is_close_ = true;
}

int HttpConn::GetFd() {
    return fd_;
}
int HttpConn::GetFd() const {
    return fd_;
}
uint16_t HttpConn::GetPort() {
    return addr_.sin_port;
}
uint16_t HttpConn::GetPort() const {
    return addr_.sin_port;
}
uint32_t HttpConn::GetIp() {
    return addr_.sin_addr.s_addr;
}
uint32_t HttpConn::GetIp() const {
    return addr_.sin_addr.s_addr;
}
sockaddr_in HttpConn::GetAddr() {
    return addr_;
}
sockaddr_in HttpConn::GetAddr() const {
    return addr_;
}
void HttpConn::Close() {
    if(!is_close_) {
        assert(close(fd_) == 0);
        fd_ = -1;
        addr_ = {0};
        is_close_ = true;
        user_cnt_--;
    }
}