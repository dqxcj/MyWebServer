#ifndef SRC_INCLUDE_HTTPRESPONSE_H_
#define SRC_INCLUDE_HTTPRESPONSE_H_

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap
#include <string>


class Buffer;

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();
    void Init(const std::string& src_dir, std::string& path, bool isKeepAlive = false, int code = -1);
    void MakeResponse(Buffer *buff);
    void UnmapFile();
    char* File();
    size_t FileLen();
    void ErrorContent(Buffer *buff, const std::string &message);
    int code() const { return code_; }

private:
    void AddStateLine_(Buffer *buff);
    void AddHeader_(Buffer *buff);
    void AddContent_(Buffer *buff);
    void ErrorHtml_();
    std::string GetFileType_();
    int code_;
    bool isKeepAlive_;  //是否是长连接
    
    std::string path_;      // 相对根目录的路径
    std::string src_dir_;   // 根目录

    char* mm_file_; //指向mmap映射的起始地址
    struct stat mm_file_stat_;  // 用户请求的文件的stat

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;  // 后缀 - 类型
    static const std::unordered_map<int, std::string> CODE_STATUS;    // 状态码 - 描述 
    static const std::unordered_map<int, std::string> CODE_PATH;      // 状态码 - 路径
};

#endif  // SRC_INCLUDE_HTTPRESPONSE_H_