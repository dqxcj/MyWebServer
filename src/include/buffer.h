#ifndef BUFFER_H
#define BUFFER_H

#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>

class Buffer {
public:
    Buffer(int initBufferSize = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;

    const char* Peek() const;

    const char* BeginWriteConst();
    char* BeginWrite();

    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveAll();
    void RetrieveUntil(const char* end);

    void Append(const std::string& str);
    void Append(const void* data, size_t len);
    void Append(const char* str, size_t len);

    ssize_t ReadFd_(int fd);
private:
    char* BeginPtr_();
    const char* BeginPtr_() const;

    void MakeSpace_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> read_pos_;  // 读指针
    std::atomic<std::size_t> write_pos_; // 写指针
};

#endif  // BUFFER_H