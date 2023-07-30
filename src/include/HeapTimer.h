#ifndef SRC_INCLUDE_HEAPTIMER_H_
#define SRC_INCLUDE_HEAPTIMER_H_

#include "Macros.h"
#include "MinHeapWithHash.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cassert>

using Clock = std::chrono::high_resolution_clock;  
using Ms = std::chrono::milliseconds;   // 毫秒
using TimePoint = Clock::time_point;    // 时间点类型

struct TimeNode {
    int socket_fd_;                            // 对应的socket_fd_
    TimePoint time_out_;                       // 超时时间
    std::function<void()> time_out_callback_;  // 超时回调函数

    bool operator<(const TimeNode &rnode) {
        return time_out_ < rnode.time_out_;
    }
};

class HeapTimer {
public:
    HeapTimer();

    ~HeapTimer();

    // 无拷贝和移动操作
    DELETE_COPY_AND_MOVE(HeapTimer);

    // 新增节点
    void Add(TimeNode node);

    // 获取距离下一次超时时间的时间间隔
    int GetNextOutTime();

    bool Empty();

   private:
    std::vector<TimeNode> heap_;                          // 用vector来模拟堆
    std::unordered_map<int, size_t> fd_to_index_;         // 通过fd快速找到vector索引

public:
    // 获取顶部节点
    TimeNode Top();
    // 弹出顶部节点
    void Pop();
    // 向下调整
    void SiftDown(size_t);
    // 向上调整
    void SiftUp(size_t);
    // 调整指定节点
    void Adjust(size_t index);

    // 清除超时节点
    void ClearOutTimeNode();
};

#endif  // SRC_INCLUDE_HEAPTIMER_H_