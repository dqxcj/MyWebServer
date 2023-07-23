#ifndef SRC_INCLUDE_HEAP_H_
#define SRC_INCLUDE_HEAP_H_

#include <vector>
#include <unordered_map>

template <typename T>
class Heap {
public:
    explicit Heap()=default;

    // 向下调整
    void SiftDown();
    // 向上调整
    void SiftUp();
    // 新增节点
    void Add(T node) {
        
    }
    // 获取顶部节点
    T Top();
    // 弹出顶部节点
    void Pop();

private:
    std::vector<T> heap_;                            // 用vector来模拟堆
    std::unordered_map<int, size_t> fd_to_index_;    // 通过fd快速找到vector索引
};

#endif  // SRC_INCLUDE_HEAP_H_