#ifndef SRC_INCLUDE_MINHEAPWITHHASH_
#define SRC_INCLUDE_MINHEAPWITHHASH_

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "Macros.h"

template <typename Node, typename Key>
class MinHeapWithHash {
public:
    MinHeapWithHash()=default;
    ~MinHeapWithHash()=default;

    DELETE_COPY_AND_MOVE(MinHeapWithHash);

    // 新增节点
    void Add(Node node, Key key);
    // 获取顶部节点
    Node Top();
    // 弹出顶部节点
    void Pop();
    // 判空
    bool Empty();
    // 清空小根堆
    void Clear();

   private:
    std::vector<Node> heap_;                        // 用vector来模拟堆
    std::unordered_map<Key, size_t> key_to_index_;  // 通过key快速找到vector索引
    std::unordered_map<size_t, Key> index_to_key_;  // 通过vector索引快速找到key

    // 向下调整
    void SiftDown(size_t index);
    // 向上调整
    void SiftUp(size_t index);
    // 交换两个节点的内容，调整key_to_index_和index_to_key_，但不调整heap_
    void Swap(size_t index1, size_t index2);
    // 调整指定节点
    void Adjust(size_t index);
};

// 新增节点
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::Add(Node node, Key key) {
    if (key_to_index_.find(key) != key_to_index_.end()) {
        heap_[key_to_index_[key]] = node;
        Adjust(key_to_index_[key]);
    } else {
        heap_.emplace_back(node);
        key_to_index_[key] = heap_.size() - 1;
        index_to_key_[key_to_index_[key]] = key;
        Adjust(heap_.size() - 1);
    }
}

// 获取顶部节点
template <typename Node, typename Key>
Node MinHeapWithHash<Node, Key>::Top() {
    assert(!heap_.empty());
    return heap_.front();
}
// 删除顶部节点
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::Pop() {
    assert(!heap_.empty());
    auto old_sz = heap_.size();
    heap_[0] = heap_[old_sz - 1];
    heap_.resize(old_sz - 1);
    Adjust(0);
}

// 判空
template <typename Node, typename Key>
bool MinHeapWithHash<Node, Key>::Empty() {
    return heap_.empty();
}

// 清空小根堆
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::Clear() {
    heap_.clear();
    key_to_index_.clear();
    index_to_key_.clear();
}

// 向下调整
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::SiftDown(size_t index) {
    if (index < 0 || index >= heap_.size()) {
        return;
    }

    while (true) {
        size_t left = index * 2;
        size_t right = index * 2 + 1;

        if(index == 0) {
            left = 1;
            right = 2;
        }

        if (left >= heap_.size()) {
            break;  // heap_[index] 无子节点
        }

        if (right >= heap_.size()) {
            right = left;  // heap_[index] 只有左子节点
        }

        size_t min_child = left;
        if (heap_[right] < heap_[min_child]) {
            min_child = right;
        }

        if (heap_[min_child] < heap_[index]) {
            Swap(index, min_child);
        } else {
            break;  // heap_[index]的子节点都比自己大
        }

        index = min_child;
    }
}
// 向上调整
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::SiftUp(size_t index) {
    if (index < 0 || index >= heap_.size()) {
        return;
    }

    while (true) {
        size_t father = index / 2;

        if (index == 0 || heap_[father] < heap_[index]) {
            break;
        }

        if (heap_[index] < heap_[father]) {
            Swap(index, father);
        }

        index = father;
    }
}

// 交换两个节点的内容，调整key_to_index_和index_to_key_，但不调整heap_
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::Swap(size_t index1, size_t index2) {
    if(index1 >= heap_.size() || index2 >= heap_.size()) {
        return ;
    }

    std::swap(heap_[index1], heap_[index2]);

    auto key1 = index_to_key_[index1];
    auto key2 = index_to_key_[index2];

    key_to_index_[key1] = index2;
    key_to_index_[key2] = index1;
    index_to_key_[index1] = key2;
    index_to_key_[index2] = key1;
}

// 调整指定节点
template <typename Node, typename Key>
void MinHeapWithHash<Node, Key>::Adjust(size_t index) {
    if (index < 0 || index >= heap_.size()) {
        return;
    }

    SiftUp(index);
    SiftDown(index);
}

#endif  // SRC_INCLUDE_MINHEAPWITHHASH_