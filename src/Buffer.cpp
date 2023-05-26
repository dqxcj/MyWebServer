#include "Buffer.h"

Buffer::Buffer() {}

void Buffer::SetBuf(const std::string &s) { buf_ = s; }

void Buffer::Write(const char *s, int sz) {
  for (int i = 0; i < sz; i++) {
    if (*(s + i) == '\0') break;
    buf_.push_back(*(s + i));
  }
}
const char *Buffer::CStr() { return buf_.c_str(); }

void Buffer::Clear() { buf_.clear(); }

ssize_t Buffer::Size() { return buf_.size(); }
