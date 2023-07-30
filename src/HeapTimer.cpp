#include "HeapTimer.h"
#include <cassert>
#include <cstddef>
#include <type_traits>
#include "HttpRequest.h"

HeapTimer::HeapTimer()=default;

HeapTimer::~HeapTimer()=default;

// 新增节点
void HeapTimer::Add(TimeNode node) {
    if(fd_to_index_.find(node.socket_fd_) != fd_to_index_.end()) {
        heap_[fd_to_index_[node.socket_fd_]] = node;
        Adjust(fd_to_index_[node.socket_fd_]);
    } else {
        heap_.emplace_back(node);
        fd_to_index_[node.socket_fd_] = heap_.size() - 1;
        Adjust(heap_.size() - 1);
    }
}
// 获取顶部节点
TimeNode HeapTimer::Top() {
    assert(!heap_.empty());
    return heap_.front();
}
// 删除顶部节点
void HeapTimer::Pop() {
    assert(!heap_.empty());
    auto old_sz = heap_.size();
    heap_[0] = heap_[old_sz - 1];
    heap_.resize(old_sz - 1);
    Adjust(0);
}
// 向下调整
void HeapTimer::SiftDown(size_t index) {
    if(index >= heap_.size()) {
        return ;
    }

    while(true) {
        size_t left = index * 2;
        size_t right = index * 2 + 1;

        if(left >= heap_.size()) {
            break;      // heap_[index] 无子节点
        }

        if(right >= heap_.size()) {
            right = left;   // heap_[index] 只有左子节点
        }

        size_t min_child = left;
        if(heap_[right] < heap_[min_child]) {
            min_child = right;
        }

        if(heap_[min_child] < heap_[index]) {
            std::swap(heap_[index], heap_[min_child]);
            fd_to_index_[heap_[index].socket_fd_] = index;
            fd_to_index_[heap_[min_child].socket_fd_] = min_child;
        } else {
            break;      // heap_[index]的子节点都比自己大
        }
    }
}
// 向上调整
void HeapTimer::SiftUp(size_t index) {
    if (index >= heap_.size()) {
        return;
    }

    while(true) {
        size_t father = index / 2;

        if (index == 0 || heap_[father] < heap_[index]) {
            break;
        }

        if(heap_[index] < heap_[father]) {
            std::swap(heap_[index], heap_[father]);
            fd_to_index_[heap_[index].socket_fd_] = index;
            fd_to_index_[heap_[father].socket_fd_] = father;
        }

        index = father;
    }
}

// 调整指定节点
void HeapTimer::Adjust(size_t index) {
    if (index >= heap_.size()) {
        return;
    }

    SiftUp(index);
    SiftDown(index);
}

// 清除超时节点
void HeapTimer::ClearOutTimeNode() {
    if (heap_.empty()) {
        return;
    }
    while (!heap_.empty()) {
        TimeNode node = heap_.front();
        if (std::chrono::duration_cast<Ms>(node.time_out_ - Clock::now()).count() > 0) {
            break;
        }
        node.time_out_callback_();
        Pop();
    }
}

// 获取距离下一次超时时间的时间间隔
int HeapTimer::GetNextOutTime() {
    ClearOutTimeNode();
    int res = -1;
    if (!heap_.empty()) {
        res = int(std::chrono::duration_cast<Ms>(heap_.front().time_out_ - Clock::now()).count());
        if (res < 0) {
            res = 0;
        }
    }
    return res;

    
}

bool HeapTimer::Empty() {
    return heap_.empty();
}