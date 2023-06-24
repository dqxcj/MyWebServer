#ifndef SRC_INCLUDE_HTTPCONN_H_
#define SRC_INCLUDE_HTTPCONN_H_

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <unistd.h>
#include <errno.h>      
#include <assert.h>
#include <atomic>
#include <string>

class HttpRequest;
class HttpResponse;
class Buffer;

class HttpConn {
public:
    HttpConn();
    ~HttpConn();
    void init(int sockFd);
    void Close();
    int GetFd() const;

    ssize_t Read();
    ssize_t Write();

    bool process();

    // 要写的字节数
    int ToWriteBytes();

    bool IsKeepAlive() const;

    void SetIsET(bool is_ET);

    static std::atomic<int> userCount;
    static bool isET;
private:    
    int fd_;    // 该连接的fd

    int iov_cnt_;   // iov数量
    struct iovec iov_[2];   // iov数组

    Buffer *read_buffer_{nullptr};    // 读buf
    Buffer *write_buffer_{nullptr};    // 写buf

    HttpRequest *request_{nullptr};   // http请求解析相关
    HttpResponse *response_{nullptr}; // http响应相关

    bool is_close_;

    std::string src_dir_;
};

#endif  // SRC_INCLUDE_HTTPCONN_H_