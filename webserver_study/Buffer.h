#ifndef BUFFER_H
#define BUFFER_H
#include <string>

class Buffer {
public:
    Buffer();

    void Write(const char *s, int sz);
    const char *CStr();
    void Clear();
    void SetBuf(const std::string &s);
    ssize_t Size();
private:
    std::string buf_;
};

#endif 