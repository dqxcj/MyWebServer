#include "MinHeapWithHash.h"
#include <iostream>
using namespace std;

void Output(MinHeapWithHash<int, int> &heap, vector<int> &nums);

int main() {
    MinHeapWithHash<int, int> heap{};
    vector<int> nums1 = {3, 2, 1, 6, 5, 4, 8, 7, 9, 0};  // 测试排序能力
    vector<int> nums2 = {3, 2, 1, 3, 2, 1};  // 测试相同的值是否会对其功能产生影响
    vector<int> nums3 = {-9, -7, -3, -8, -6, 5, 4, 2, 1, 0};  // 测试负值

    cout << "测试排序能力" << endl;
    heap.Clear();
    Output(heap, nums1);
    cout << endl << endl;


    cout << "测试相同的值是否会对其功能产生影响" << endl;
    heap.Clear();
    Output(heap, nums2);
    cout << endl << endl;


    cout << "测试负值" << endl;
    heap.Clear();
    Output(heap, nums3);
    
    
    cout << endl;
    return 0;
}

void Output(MinHeapWithHash<int, int> &heap, vector<int> &nums) {
    for (int i = 0; i < nums.size(); i++) {
        heap.Add(nums[i], i);
    }

    while (!heap.Empty()) {
        cout << heap.Top() << " ";
        heap.Pop();
    }
}