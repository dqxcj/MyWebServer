#include "HttpConn.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "buffer.h"

const char* HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;

bool HttpConn::isET = false;

HttpConn::HttpConn(): 
    fd_(-1), addr_({0}), is_close_(true)
    {}

HttpConn::~HttpConn() {
    Close();
}

// 初始化
void HttpConn::init(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    fd_ = fd;
    addr_ = addr;
    userCount++;
    is_close_ = false;
}

// 关闭连接
void HttpConn::Close() {
    if (is_close_ == false) {
        is_close_ = true;
        userCount--;
        close(fd_);
    }
}

// 返回该连接的fd_
int HttpConn::GetFd() const {
    return fd_;
}

// 返回该连接的端口
int HttpConn::GetPort() const {
    return addr_.sin_port;
}

// 返回该连接的ip地址
const char* HttpConn::GetIp() const {
    return inet_ntoa(addr_.sin_addr);
}

// 返回该连接的地址
sockaddr_in HttpConn::GetAddr() const {
    return addr_;
}

// 读数据到读缓冲区
ssize_t HttpConn::read() {
    ssize_t len = 0;
    // do-while，非ET模式只运行一次，ET模式则循环
    do {
        ssize_t tmp = read_buffer_->ReadFd_(fd_);
        if (tmp <= 0) {
            break;
        }
        len += tmp;
    } while (isET);
    return len;
}

ssize_t HttpConn::write() {
    ssize_t len = -1;
    // 采用do-while，非ET模式只写一次，ET模式循环写
    do {
        // 分散写
        len = writev(fd_, iov_, iov_cnt_);
        if (len <= 0) {
            break;
        }

        // 写完了
        if (iov_[0].iov_len + iov_[1].iov_len == 0) { break; }
        
        // 开始写第二个iov了
        if (static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t*)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if (iov_[0].iov_len) {
                write_buffer_->RetrieveAll();
                iov_[0].iov_len = 0;
            }            
        } else {    // 还在写第一个iov
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            write_buffer_->Retrieve(len);
        }
    } while (isET || ToWriteBytes() > 10240);
    return len;
}

// 处理读取到的数据，制作响应，为发送响应做准备
bool HttpConn::process() {
    request_->Init();

    if (read_buffer_->ReadableBytes() <= 0) {
        return false;
    }
    if (request_->parse(read_buffer_)) {
        printf("%s\n", request_->path().c_str());
        response_->Init(srcDir, request_->path(), request_->IsKeepAlive(), 200);
    } else {  // parse failed
        response_->Init(srcDir, request_->path(), false, 400);
    }
    response_->MakeResponse(write_buffer_);

    iov_[0].iov_base = const_cast<char*>(write_buffer_->Peek());
    iov_[0].iov_len = write_buffer_->ReadableBytes();
    iov_cnt_ = 1;
    if (response_->FileLen() > 0 && response_->File()) {
        iov_[1].iov_base = response_->File();
        iov_[1].iov_len = response_->FileLen();
        iov_cnt_ = 2;
    }
    printf("filesize: %ld %d, to %d\n", response_->FileLen(), iov_cnt_, ToWriteBytes());
    return true;
}

void HttpConn::SetIsET(bool is_ET) {
    isET = is_ET;
}

// 要写的字节数
int HttpConn::ToWriteBytes() { 
    return iov_[0].iov_len + iov_[1].iov_len; 
}

bool HttpConn::IsKeepAlive() const { 
    return request_->IsKeepAlive(); 
}