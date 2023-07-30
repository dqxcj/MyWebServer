#include <iostream>
#include "HeapTimer.h"

int main() {
    std::vector<TimeNode> time_nodes(10);
    for(int i = 0; i < 10; i++) {
        time_nodes[i].time_out_ = Clock::now();
    }

    std::cout << "**********打乱前**********" << std::endl;
    TimePoint timepoint1 = Clock::now();
    for(auto &node : time_nodes) {
        std::cout << std::chrono::duration_cast<Ms>(timepoint1 - node.time_out_).count() << std::endl;
    }

    std::swap(time_nodes[0], time_nodes[7]);
    std::swap(time_nodes[3], time_nodes[0]);
    std::swap(time_nodes[1], time_nodes[9]);
    std::swap(time_nodes[5], time_nodes[3]);
    std::swap(time_nodes[7], time_nodes[2]);
    std::swap(time_nodes[4], time_nodes[6]);
    std::swap(time_nodes[3], time_nodes[0]);
    std::swap(time_nodes[8], time_nodes[0]);

    std::cout << "**********打乱后**********" << std::endl;
    TimePoint timepoint2 = Clock::now();
    for (auto &node : time_nodes) {
        std::cout << std::chrono::duration_cast<Ms>(timepoint2 - node.time_out_).count() << std::endl;
    }

    HeapTimer *heap_timer = new HeapTimer();
    for(auto &node : time_nodes) {
        heap_timer->Add(node);
    }

    std::cout << "**********入堆后**********" << std::endl;
    TimePoint timepoint3 = Clock::now();
    while(!heap_timer->Empty()) {
        std::cout << std::chrono::duration_cast<Ms>(timepoint3 - heap_timer->Top().time_out_).count() << std::endl;
        heap_timer->Pop();
    }

    return 0;
}