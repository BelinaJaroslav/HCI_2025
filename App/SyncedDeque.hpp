#pragma once

#include <deque>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable


template<typename T>
class SyncedDeque {
protected:
    std::mutex mux;
    std::deque<T> de_queue;
    std::condition_variable cv_sleep;
    std::mutex mux_sleep;
    std::atomic<bool> shutdown_ = false;

public:
    SyncedDeque() = default;
    SyncedDeque(const SyncedDeque<T>&) = delete;
    virtual ~SyncedDeque() {
        clear();
    }

    
    // Returns and maintains item at front of Queue
    const T& front() {
        std::scoped_lock lock(mux);
        return de_queue.front();
    }

    
    // Returns and maintains item at back of Queue
    const T& back() {
        std::scoped_lock lock(mux);
        return de_queue.back();
    }

    
    // Removes and returns item from front of Queue
    T pop_front() {
        std::scoped_lock lock(mux);
        auto t = std::move(de_queue.front());
        de_queue.pop_front();
        return t;
    }

    
    // 
    T pop_front_wait() {
        wait(); // blocks until not empty
        std::scoped_lock lock(mux);
        T t = std::move(de_queue.front());
        de_queue.pop_front();
        return t;
    }

    
    // Removes and returns item from back of Queue
    T pop_back() {
        std::scoped_lock lock(mux);
        auto t = std::move(de_queue.back());
        de_queue.pop_back();
        return t;
    }

    
    // Adds an item to back of Queue
    void push_back(const T& item) {
        std::scoped_lock lock(mux);
        de_queue.emplace_back(std::move(item));

        std::unique_lock<std::mutex> ul(mux_sleep);
        cv_sleep.notify_one();
    }

    
    // Adds an item to front of Queue
    void push_front(const T& item) {
        std::scoped_lock lock(mux);
        de_queue.emplace_front(std::move(item));

        std::unique_lock<std::mutex> ul(mux_sleep);
        cv_sleep.notify_one();
    }

    
    // Returns true if Queue has no items
    bool empty() {
        std::scoped_lock lock(mux);
        return de_queue.empty();
    }

    
    // Returns number of items in Queue
    size_t size() {
        std::scoped_lock lock(mux);
        return de_queue.size();
    }

    
    // Clears Queue
    void clear() {
        std::scoped_lock lock(mux);
        de_queue.clear();
    }

    
    // If empty wait for fill-up and wake-up
    void wait() {
        while (empty()) {
            std::unique_lock<std::mutex> ul(mux_sleep);
            cv_sleep.wait(ul);
        }
    }

    // If neccessary, add unimplemented methods from std::deque
};
