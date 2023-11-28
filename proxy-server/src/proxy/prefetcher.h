#pragma once

#include "../proxy/queue.h"
#include "../http/http_handler.h"

#include <string>

class Prefetcher {
    private:
        Prefetcher();

        // Private copy constructor and copy assignment operator to prevent cloning
        Prefetcher(const Prefetcher&) = delete;
        Prefetcher& operator=(const Prefetcher&) = delete;

        std::string constructRequestString(const std::string &);

    public:
        static Prefetcher * getInstance();

        void prefetch(Queue<std::string> *, HttpHandler *);
        bool shouldPrefetch(const std::string &);
};