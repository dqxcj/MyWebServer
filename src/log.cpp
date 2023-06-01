#include "log.h"
#include "blockqueue.h"
#include "buffer.h"

Log::Log() {
    lineCount_ = 0;
    isAsync_ = false;
    writeThread_ = nullptr;
    deque_ = nullptr;
    toDay_ = 0;
    fp_ = nullptr;
}

// 获取当前的日志层级
int Log::GetLevel() {
    std::lock_guard<std::mutex> locker(mtx_);
    return level_;
}

// 设置日志层级
void Log::SetLevel(int level) {
    std::lock_guard<std::mutex> locker(mtx_);
    level_ = level;
}

// 初始化阻塞队列、异步线程、日志文件
void Log::init(int level, const char* path, const char* suffix, int maxQueueSize) {
    isOpen_ = true;
    level_ = level;
    if (maxQueueSize > 0) {
        isAsync_ = true;
        if (!deque_) {
            std::unique_ptr<BlockDeque<std::string>> newDeque(new BlockDeque<std::string>);
            deque_ = std::move(newDeque);    // 阻塞队列

            std::unique_ptr<std::thread> newThread(new std::thread(FlushLogThread));
            writeThread_ = std::move(newThread);    // 异步线程
        }
    } else {
        isAsync_ = false;
    }
    lineCount_ = 0;

    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;

    path_ = path;
    suffix_ = suffix;
    // 构建文件名
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s",
             path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix);
    
    toDay_ = t.tm_mday;

    // 创建日志文件
    {
        std::lock_guard<std::mutex> locker(mtx_);
        buff_.RetrieveAll();
        if (fp_) {
            flush();
            fclose(fp_);
        }
        fp_ = fopen(fileName, "a");
        if (fp_ == nullptr) {
            mkdir(path_, 0777);
            fp_ = fopen(fileName, "a");
        }
        assert(fp_ != nullptr);
    }
}

// 将日志信息写入阻塞队列
void Log::write(int level, const char *format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    // 新的一天或日志已写满，需要创建新的日志文件
    if (toDay_ != t.tm_mday || (lineCount_ && (lineCount_ % MAX_LINES == 0))) {
        std::unique_lock<std::mutex> locker(mtx_);
        locker.unlock();

        // 构建新文件名
        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
        if (toDay_ != t.tm_mday) {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
            toDay_ = t.tm_mday;
            lineCount_ = 0;
        } else {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail, (lineCount_ / MAX_LINES), suffix_);
        }

        // 打开新文件
        locker.lock();
        flush();
        fclose(fp_);
        fp_ = fopen(newFile, "a");
        assert(fp_ != nullptr);            
    }
    // 构造并写入日志到阻塞队列中
    {
        std::unique_lock<std::mutex> locker(mtx_);
        lineCount_++;
        // 每条日志的写入时间
        int n = snprintf(buff_.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        buff_.HasWritten(n);
        // 每条日志的类型
        AppendLogLevelTitle_(level);
        // 日志的具体内容
        va_start(vaList, format);
        int m = vsnprintf(buff_.BeginWrite(), buff_.WritableBytes(), format, vaList);
        va_end(vaList);
        buff_.HasWritten(m);
        buff_.Append("\n\0", 2);
        // 将日志添加到阻塞队列或直接写入文件
        if (isAsync_ && deque_ && !deque_->full()) {
            deque_->push_back(buff_.RetrieveAllToStr());
        } else {    // 直接写意味着这一步不是异步的，而且这一条日志可能会和其他日志时间顺序错乱
            fputs(buff_.Peek(), fp_);
        }
        // 清空buff
        buff_.RetrieveAll();
    }
}

// 为每条日志添加日志类型头部
void Log::AppendLogLevelTitle_(int level) {
    switch(level) {
    case 0:
        buff_.Append("[debug]: ", 9);
        break;
    case 1:
        buff_.Append("[info] : ", 9);
        break;
    case 2:
        buff_.Append("[warn] : ", 9);
        break;
    case 3:
        buff_.Append("[error]: ", 9);
        break;
    default:
        buff_.Append("[info] : ", 9);
        break;
    }
}

// 将阻塞队列/缓冲区里的数据写入文件中
void Log::flush() {
    if (isAsync_) {
        // 唤醒一个消费者来清理阻塞队列
        deque_->flush();
    }
    // 将缓冲区的数据写入文件中
    fflush(fp_);
}

Log::~Log() {

}

// 单例模式
Log* Log::Instance() {
    static Log inst;
    return &inst;
}

// 不断从阻塞队列中取数据并写入到文件中
// 这是日志库地异步线程地主要工作
void Log::AsyncWrite_() {
  std::string str = "";
  while (deque_->pop(str)) {
    std::lock_guard<std::mutex> locker(mtx_);
    fputs(str.c_str(), fp_);  // fp_ 为 FILE* 类型
  }
}

// 线程执行的函数
void Log::FlushLogThread() {
    Log::Instance()->AsyncWrite_();
}