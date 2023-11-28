/*
** Class Source
*/
#pragma once
#include "threadpool.h"

ThreadPool::ThreadPool() {
    num_threads_ = MAX_THREADS;
}

void ThreadPool::init(int num_threads) {
    num_threads_ = num_threads > MAX_THREADS? MAX_THREADS: num_threads;
}

template <typename Callable>
void ThreadPool::createWorkerThreads(Callable && function) {
    for (int i=0; i<num_threads_; i++) {
        threads_.emplace_back(std::thread(std::forward<Callable>(function)));
    }
}

void ThreadPool::joinWorkerThreads() {
    for (int i=0; i<threads_.size(); i++) {
        threads_.at(i).join();
    }
    std::cout << "All threads completed" << std::endl;
}