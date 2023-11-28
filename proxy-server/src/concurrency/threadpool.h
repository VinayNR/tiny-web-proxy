#pragma once

#include <vector>
#include <thread>
#include <iostream>

class ThreadPool {
    private:
        std::vector<std::thread> threads_;
        int num_threads_;
        const static int MAX_THREADS = 20;

    public:
        ThreadPool();

        void init(int);

        template <typename Callable>
        void createWorkerThreads(Callable &&);

        void joinWorkerThreads();
};