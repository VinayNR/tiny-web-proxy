#pragma once

#include "../concurrency/threadpool.cpp"
#include "../http/http_handler.h"
#include "../html/parser.h"
#include "queue.h"
#include "cache.h"
#include "prefetcher.h"


class ProxyServer {
    private:
        // number of active backlogged connections allowed
        const int SERVER_BACKLOG = 20;

        // listen tcp socket file descriptor
        int sockfd_;

        // proxy cache
        Cache<std::string, std::string> dns_cache_;
        Cache<std::string, std::vector<char>> http_cache_;

        // html parser for the proxy
        HtmlParser * html_parser;

        // prefetcher for the proxy
        Prefetcher * prefetcher;

        // prefetch uri queue
        Queue<std::string> * prefetcher_queue;

        // thread pool of 10 threads
        ThreadPool prefetcher_thread_pool;

        // thread pool of 20 threads
        ThreadPool client_thread_pool;

        // request queue
        Queue<int> * client_requests_queue;

        // http handler
        HttpHandler * http_handler;

        void processClientRequests(Queue<int> *, HttpHandler *);
    
    public:
        ProxyServer();

        int setupSocket(char *);
        void start();
        void shutdown();
};