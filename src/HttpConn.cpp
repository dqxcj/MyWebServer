#include "HttpConn.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "buffer.h"
#include "unistd.h"
#include "log.h"

std::atomic<int> HttpConn::userCount;

bool HttpConn::isET = false;

HttpConn::HttpConn()
    : fd_(-1),
      is_close_(true),
      read_buffer_(new Buffer(1024)),
      write_buffer_(new Buffer(1024)),
      request_(new HttpRequest()),
      response_(new HttpResponse()),
      src_dir_("/home/ljy/MyWebServer") {}

HttpConn::~HttpConn() {
    Close();
    delete read_buffer_;
    delete write_buffer_;
    delete request_;
    delete response_;
}

// 初始化
void HttpConn::init(int fd) {
    assert(fd > 0);
    fd_ = fd;
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
         writev(1, iov_, iov_cnt_);
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
    std::cout << "ccc" << std::endl;
    if (request_->parse(read_buffer_)) {
        std::cout << request_->path() << std::endl;
        response_->Init(src_dir_, request_->path(), request_->IsKeepAlive(), 200);
    } else {  // parse failed
        response_->Init(src_dir_, request_->path(), false, 400);
    }
    std::cout << "bbb" << std::endl;
    response_->MakeResponse(write_buffer_);
    std::cout << "aaa" << std::endl;
    iov_[0].iov_base = const_cast<char*>(write_buffer_->Peek());
    iov_[0].iov_len = write_buffer_->ReadableBytes();
    iov_cnt_ = 1;
    if (response_->FileLen() > 0 && response_->File()) {
        iov_[1].iov_base = response_->File();
        iov_[1].iov_len = response_->FileLen();
        iov_cnt_ = 2;
    }
    LOG_INFO("filesize: %ld %d, to %d\n", response_->FileLen(), iov_cnt_, ToWriteBytes());
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