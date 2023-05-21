#ifndef HTTPCONN_H
#define HTTPCONN_H
#include <arpa/inet.h>  // sockaddr_in
#include <assert.h> // assert
#include <unistd.h>
#include <atomic>

class HttpConn {
public:
    static std::atomic<int> user_cnt_;
    HttpConn();
    ~HttpConn();
    void Open(int fd, sockaddr_in addr);
    int GetFd();
    int GetFd() const;
    uint16_t GetPort();
    uint16_t GetPort() const;
    uint32_t GetIp();
    uint32_t GetIp() const;
    sockaddr_in GetAddr();
    sockaddr_in GetAddr() const;
    void Close();
private:
    int fd_;
    sockaddr_in addr_;
    bool is_close_;
};

#endif