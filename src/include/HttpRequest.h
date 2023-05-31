#ifndef SRC_INCLUDE_HTTPREQUEST_H_
#define SRC_INCLUDE_HTTPREQUEST_H_

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
// #include <mysql/mysql.h>  //mysql

class Buffer;

class HttpRequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,   // 正在解析请求首行
        HEADERS,        // 头
        BODY,           // 体
        FINISH,         // 完成
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    HttpRequest() { Init(); }
    ~HttpRequest() = default;
    void Init();
    bool parse(Buffer *buff);

    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;

    bool IsKeepAlive() const;

private:
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);

    void ParsePath_();
    void ParsePost_();

    PARSE_STATE state_; //解析状态
    std::string method_, path_, version_, body_;    // 请求方法，请求路径，HTTP版本，请求体
    std::unordered_map<std::string, std::string> header_;   // 记录请求头部字段和其值
    std::unordered_map<std::string, std::string> post_;     // 
    
    static const std::unordered_set<std::string> DEFAULT_HTML;  // 默认页面地址
};

#endif  // SRC_INCLUDE_HTTPREQUEST_H_