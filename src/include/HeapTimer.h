#ifndef SRC_INCLUDE_HEAPTIMER_H_
#define SRC_INCLUDE_HEAPTIMER_H_

#include "Macros.h"

#include <chrono>
#include <functional>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;    // 时间点类型

class HeapTimer {
public:
    HeapTimer();

    // 无拷贝和移动操作
    DELETE_COPY_AND_MOVE(HeapTimer);


private:
    struct TimeNode {
        int socket_fd_;                             // 对应的socket_fd_
        TimePoint time_out_;                        // 超时时间
        std::function<void()> time_out_callback_;   // 超时回调函数 
    };


};

#endif  // SRC_INCLUDE_HEAPTIMER_H_