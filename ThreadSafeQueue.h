//
// Created by erick on 1/1/19.
//

#ifndef FINALTASK_THREADSAFEQUEUE_H
#define FINALTASK_THREADSAFEQUEUE_H

#include <deque>
#include <condition_variable>
#include <mutex>
#include <utility>

template<class T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue() = default;

    void push(const T &elem);  // 复制到队尾

    void move_back(T &&elem);  // 移动到队尾

    T pop(); // 元素出队

    bool empty();  // 空队列判断

protected:
    std::mutex mtx;
    std::deque<T> elemets;
};

template<class T>
void ThreadSafeQueue<T>::push(const T &elem) {
    std::unique_lock<std::mutex> lock(mtx);
    elemets.push_back(elem);
}

template<class T>
void ThreadSafeQueue<T>::move_back(T &&elem) {
    std::unique_lock<std::mutex> lock(mtx);
    elemets.emplace_back(std::move(elem));
}

template<class T>
T ThreadSafeQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mtx);
    auto elem = std::move(elemets.front());
    elemets.pop_front();
    return elem;
}

template<class T>
bool ThreadSafeQueue<T>::empty() {
    std::unique_lock<std::mutex> lock(mtx);
    return elemets.empty();
}


#endif //FINALTASK_THREADSAFEQUEUE_H
