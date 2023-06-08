#include "HttpResponse.h"
#include "buffer.h"
#include "log.h"

// 文件后缀 对应的 MIME-TYPE类型，用在响应头Content-Type中
const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

// 响应状态码对应的描述语
const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

// 响应码对应的资源路径
const std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

HttpResponse::HttpResponse() {
    code_ = -1;
    path_ = src_dir_ = "";
    isKeepAlive_ = false;
    mm_file_ = nullptr;
    mm_file_stat_ = { 0 };
    std::cout << "response construct" << std::endl;
}

HttpResponse::~HttpResponse() {
    UnmapFile();
}

// 初始化响应
void HttpResponse::Init(const std::string &src_dir, std::string &path, bool isKeepAlive, int code) {
  LOG_INFO("response init**************************************** %s", path.c_str());
  assert(!src_dir.empty());
  if (mm_file_) {
    UnmapFile();
  }

  code_ = code;
  isKeepAlive_ = isKeepAlive;
  path_ = path;
  src_dir_ = src_dir;
  mm_file_ = nullptr;
  mm_file_stat_ = {0};
}

void HttpResponse::MakeResponse(Buffer *buff) {
    // 获取文件内容并做出相应措施
    if (stat((src_dir_ + path_).data(), &mm_file_stat_) < 0 || S_ISDIR(mm_file_stat_.st_mode)) {
        LOG_ERROR("stat error: %s\n", strerror(errno));
        LOG_INFO("404 %s %d", (src_dir_ + path_).c_str(), (stat((src_dir_ + path_).data(), &mm_file_stat_) < 0));
        code_ = 404;
    } else if (!(mm_file_stat_.st_mode & S_IROTH)) {    // 不可被其他用户读
        code_ = 403;
    } else if (code_ == -1) {
        code_ = 200;
    }
    std::cout << "HttpResponse::MakeResponse1 " << path_ << std::endl;
    ErrorHtml_();
    std::cout << "HttpResponse::MakeResponse2 " << path_ << std::endl;
    AddStateLine_(buff);
    std::cout << "HttpResponse::MakeResponse3 " << path_ << std::endl;
    AddHeader_(buff);
    std::cout << "HttpResponse::MakeResponse4 " << path_ << std::endl;
    AddContent_(buff);
}

// 获取“错误”的响应文件
void HttpResponse::ErrorHtml_() {
    // 这里筛去了200这种成功的状态码
    if (CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;
        stat((src_dir_ + path_).data(), &mm_file_stat_);
    }
}

// 设置响应状态行
void HttpResponse::AddStateLine_(Buffer *buff) {
    std::string status;
    if (CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff->Append("HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n");
}

// 设置响应头部
void HttpResponse::AddHeader_(Buffer *buff) {
    buff->Append("Connection: ");
    if (isKeepAlive_) {
        buff->Append("keep-alive\r\n");
        buff->Append("keep-alive: max=6, timeout=120\r\n");
    } else {
        buff->Append("close\r\n");
    }
    buff->Append("Content-type: " + GetFileType_() + "\r\n");
}

// 设置响应正文
void HttpResponse::AddContent_(Buffer *buff) {
    int src_fd = open((src_dir_ + path_).data(), O_RDONLY);
    std::cout << src_dir_ + path_ << " fd: " << src_fd << std::endl; 
    if (src_fd < 0) {
        ErrorContent(buff, "src_fd < 0 File NotFound!");
        return;
    }
    printf("file path: %s", (src_dir_ + path_).data());
    // 内存映射，获取文件内容
    int* mm_ret = (int*)mmap(0, mm_file_stat_.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (*mm_ret == -1) {
        ErrorContent(buff, "mm_ret == -1 File NotFound!");
        return;
    }
    mm_file_ = (char*)mm_ret;
    close(src_fd);
    buff->Append("Content-length: " + std::to_string(mm_file_stat_.st_size) + "\r\n\r\n");
}

// 找不到请求的文件，构造要发送的报错文件
void HttpResponse::ErrorContent(Buffer *buff, const std::string &message) {
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";
    }
    body += std::to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>MyWebServer</em></body></html>";

    buff->Append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff->Append(body);
}

// 获取响应文件类型
std::string HttpResponse::GetFileType_() {
    std::string::size_type idx = path_.find_last_of('.');
    if (idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = path_.substr(idx);
    if (SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

char* HttpResponse::File() { return mm_file_; }

size_t HttpResponse::FileLen() { return mm_file_stat_.st_size; }

// 取消mmap映射
void HttpResponse::UnmapFile() {
    if (mm_file_) {
        munmap(mm_file_, mm_file_stat_.st_size);
        mm_file_ = nullptr;
    }
}