#include "Buffer.h"

Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), read_pos_(0), write_pos_(0) {}

// 返回剩余可写的空间
size_t Buffer::WritableBytes() const {
    return buffer_.size() - write_pos_;
}

// 返回可读字节数
size_t Buffer::ReadableBytes() const {
    return write_pos_ - read_pos_;
}

// 返回已读元素的数量
size_t Buffer::PrependableBytes() const {
    return read_pos_;
}

// 返回第一个可读但未读元素的地址
const char* Buffer::Peek() const {
    return BeginPtr_() + read_pos_;
}

// 移动写指针指定长度
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    read_pos_ += len;
}

// 清空缓冲区，读写指针置0
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    read_pos_ = 0;
    write_pos_ = 0;
}
// 清空缓存区，并返回string
std::string Buffer::RetrieveAllToStr() {
  std::string str(Peek(), ReadableBytes());
  RetrieveAll();
  return str;
}

// 移动写指针到指定地址
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end);
    Retrieve(end - Peek());
}

// 返回vector首元素的地址
char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

// 返回vector首元素的地址
const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

// 返回第一个可写位置的地址
const char* Buffer::BeginWriteConst() {
    return BeginPtr_() + write_pos_;
}

// 返回第一个可写位置的地址
char* Buffer::BeginWrite() {
    return BeginPtr_() + write_pos_;
}

// 移动写指针指定长度
void Buffer::HasWritten(size_t len) {
    write_pos_ += len;
}

// 确保还可以写len个字节，不够怎获取空间
void Buffer::EnsureWriteable(size_t len) {
    if (WritableBytes() < len) {
        MakeSpace_(len);
    }
    // 可能会有多个线程，新分配的空间被另一个线程用了
    assert(WritableBytes() >= len);
}

// 清理已读元素，空出len长的空间
// 空不出就分配新的空间
void Buffer::MakeSpace_(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(write_pos_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        // 删除已读元素
        // 这一步很妙
        std::copy(BeginPtr_() + read_pos_, BeginPtr_() + write_pos_, BeginPtr_());
        read_pos_ = 0;
        write_pos_ = read_pos_ + readable;
        // 确保读写指针正确，这是因为可能会有其他线程
        assert(readable == ReadableBytes());
    }
}

// 将str拼接到缓冲区的第一个可写位置处
void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

// 将str拼接到缓冲区的第一个可写位置处
void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

// 将str拼接到缓冲区的第一个可写位置处
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

// 从fd中读取数据到缓存区中
ssize_t Buffer::ReadFd_(int fd) {
    char buffer[65535];

    struct iovec iov[2];
    const size_t writable = WritableBytes();

    iov[0].iov_base = BeginPtr_() + write_pos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buffer;
    iov[1].iov_len = sizeof(buffer);

    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        ;
    } else if (static_cast<size_t>(len) < writable) {
        write_pos_ += len;
    } else { // 这里是采取分散读的原因
        // 如果缓冲区放不下这些元素，则会分配新空间，
        // 并将临时缓冲区的内容拼接过去
        write_pos_ = buffer_.size();
        Append(buffer, len - writable);
    }
    return len;
}