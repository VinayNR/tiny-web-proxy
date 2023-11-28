#pragma once

#include <string>

#include "request.h"
#include "response.h"
#include "../utils/sockutils.h"
#include "../utils/utils.h"
#include "../utils/netutils.h"
#include "../proxy/cache.h"

class HttpHandler {
    private:
        // pointers to a common proxy cache
        Cache<std::string, std::string> * dns_cache_;
        Cache<std::string, std::vector<char>> * http_cache_;

        // reference to deny list vector - received from the proxy server
        std::vector<std::string> denylist_domains;

        HttpHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        HttpHandler(const HttpHandler&) = delete;
        HttpHandler& operator=(const HttpHandler&) = delete;

        bool isRequestValid(HttpRequest *);
        bool isAdminRequest(HttpRequest *);
        bool isBlockedSite(HttpRequest *);
        HttpServerAddr getRemoteAddress(HttpRequest *);

    public:
        
        static HttpHandler* getInstance();
        HttpResponse * handleRequest(HttpRequest *);

        void setHttpCache(Cache<std::string, std::vector<char>> *);
        void setDnsCache(Cache<std::string, std::string> *);

        std::vector<char> readData(int);
        int readData(int, char *&);
        int writeData(int, const char *, size_t);

        HttpRequest * readRequest(int);
        HttpResponse * readResponse(int);
        int writeRequest(int, HttpRequest *);
        int writeResponse(int, HttpResponse *);
};