#pragma once
#include <cstddef>
#include <vector>
#include <atomic>
#include <mutex>

template <class T>
class Buffer {
 public:
    Buffer(std::size_t capacity)
    : container(capacity+1)
    , size(capacity+1)
    , push_idx(0)
    , pop_idx(0) {};

    Buffer(const Buffer& other) {} // Ref copy constructor for reallocation, not needed when reserving enough space (only for correct compiling).

    // Can block infinitely.
    void push(T&& value) {
        //const std::lock_guard<std::mutex> lock(push_mutex);
        while (full()) {};
        //container.emplace(container.begin()+push_idx, std::move(value));
        container[push_idx] = std::move(value);
        push_idx = (push_idx+1)%size;
    }

    // Can block infinitely.
    T&& pop() {
        //const std::lock_guard<std::mutex> lock(pop_mutex);
        while (empty()) {};
        std::size_t pop_idx_copy = pop_idx;
        pop_idx = (pop_idx+1)%size;
        return std::move(container[pop_idx_copy]);
    }

    double fill_level() {
        if (pop_idx >= push_idx) {
            return (pop_idx - push_idx)/(size-1);
        } else {
            return 1 - (pop_idx - push_idx)/(size-1);
        }
    }
    
 private:
    std::vector<T> container;
    std::size_t size;
    std::atomic<std::size_t> push_idx;
    std::atomic<std::size_t> pop_idx;

    std::mutex push_mutex;
    std::mutex pop_mutex;
    
    bool empty() {
        return push_idx == pop_idx;
    }

    bool full() {
        return (push_idx+1)%size == pop_idx;
    }
};