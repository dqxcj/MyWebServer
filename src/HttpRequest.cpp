#include "HttpRequest.h"
#include "buffer.h"

const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

// 解析请求首行
bool HttpRequest::ParseRequestLine_(const std::string& line) {
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, patten)) {
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    printf("RequestLine Error\n");
    return false;
}

// 解析url
void HttpRequest::ParsePath_() {
    if (path_ == "/") {
        path_ = "/index.html";
    } else {
        for (auto& item : DEFAULT_HTML) {
            if (path_ == item) {
                path_ += ".html";
                break;
            }
        }
    }
}

// 解析一行请求头
void HttpRequest::ParseHeader_(const std::string& line) {
    std::regex patten("^([^:]*) ?(.*)$");
    std::smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    } else {
        state_ = BODY;
    }
}

// 解析请求体
void HttpRequest::ParseBody_(const std::string& line) {
    body_ = line;
    // ParsePost_();
    state_ = FINISH;
    printf("Body:%s, len:%ld\n", body_.c_str(), body_.size());
}

// 解析HTTP请求
bool HttpRequest::parse(Buffer *buff) {
    const char CRLF[] = "\r\n";
    if (buff->ReadableBytes() <= 0) {
        return false;
    }
    // 缓冲区未读完并且未达到结束状态则循环
    while (buff->ReadableBytes() && state_ != FINISH) {
        // 获取第一个CRLF的位置
        const char* lineEnd = std::search(buff->Peek(), buff->BeginWriteConst(), CRLF, CRLF + 2);
        // 构造一行数据并处理
        std::string line(buff->Peek(), lineEnd);
        switch(state_) 
        {
        case REQUEST_LINE:  // 解析请求首行
            if (!ParseRequestLine_(line)) {
                return false;
            }
            ParsePath_();
            break;
        case HEADERS:   // 解析请求头
            ParseHeader_(line);
            if (buff->ReadableBytes() <= 2) {
                state_ = FINISH;
            }
            break;
        case BODY:  //解析请求体
            ParseBody_(line);
            break;
        default:
            break;
        }
        if (lineEnd == buff->BeginWrite()) { break; }
        buff->RetrieveUntil(lineEnd + 2);
    }
    printf("[%s], [%s], [%s]\n", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

std::string HttpRequest::path() const { return path_; }

std::string& HttpRequest::path() { return path_; }

std::string HttpRequest::method() const { return method_; }

std::string HttpRequest::version() const  { return version_; }

std::string HttpRequest::GetPost(const std::string& key) const {
    assert(key != "");
    if (post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}

std::string HttpRequest::GetPost(const char* key) const {
    assert(key != nullptr);
    if (post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}

bool HttpRequest::IsKeepAlive() const {
    if (header_.count("Connection") == 1) {
        // 注意1.1以上版本才支持长连接
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}