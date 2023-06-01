#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/stat.h>

class BlockDeque;
class Buffer;

class Log {
public:
    void init(int level = 1, const char* path = "./log",
              const char* suffix = ".log",
              int maxQueueCapacity = 1024);
    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char *format, ...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }

    Log (const Log &) = delete;
    Log& operator=(const Log &) = delete;

private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();
private:
    static const int LOG_PATH_LEN = 256;  // 日志路径的最大长度
    static const int LOG_NAME_LEN = 256;  // 日志名称的最大长度
    static const int MAX_LINES = 50000;   // Log 文件的最大行数
    
    const char* path_;  // 日志文件路径
    const char* suffix_;    // 日志文件后缀

    int lineCount_; // 行数统计，超过MAX_LINES则会建立新的日志文件
    int toDay_;     // 记录当前日期

    bool isOpen_;    

    Buffer buff_;   // 缓存
    int level_;     // 日志层级
    bool isAsync_;  // 是否异步

    FILE* fp_;  // 日志文件指针
    std::unique_ptr<BlockDeque<std::string>> deque_;    // string格式的日志
    std::unique_ptr<std::thread> writeThread_;  // 异步写线程
    std::mutex mtx_;    // 锁
};

// level = 1
// LOG_DEBUG  0 <= 1  y
// LOG_INFO   1 <= 1  y
// LOG_WARN   2 <= 1  n
// LOG_ERROR  3 <= 1  n
#define LOG_BASE(level, format, ...) \
    do { \
        Log* log = Log::Instance(); \
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush(); \
        } \
    } while (0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...)  do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...)  do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif  // LOG_H