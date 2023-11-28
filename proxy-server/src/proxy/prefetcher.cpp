#include "prefetcher.h"

#include <iostream>

#include "../http/request.h"

// default constructor
Prefetcher::Prefetcher() {
}

Prefetcher * Prefetcher::getInstance() {
    static Prefetcher instance;
    return &instance;
}

bool Prefetcher::shouldPrefetch(const std::string & uri) {
    return !(uri[0] == '#' || uri.substr(0,4) == "http");
}

void Prefetcher::prefetch(Queue<std::string> * prefetcher_queue, HttpHandler * http_handler) {

    std::cout << " ------ Prefetcher Thread Pool: " << std::this_thread::get_id() << " ------ " << std::endl;

    while (true) {
        // take a resource from the queue to fetch
        std::string resource = prefetcher_queue->dequeue();
        
        // if the URI to prefetch is valid
        if (resource != "") {
            std::cout << std::this_thread::get_id() << " obtained a URI to prefetch: " << resource << std::endl;

            // construct a simple get request
            HttpRequest *http_request = nullptr;
            if ((http_request = HttpRequest::build(constructRequestString(resource))) != nullptr) {
                // send the request, which also updates the cache
                http_handler->handleRequest(http_request);
            }
        }
    }
}

std::string Prefetcher::constructRequestString(const std::string& resource) {    
    // Find the position where the host ends
    size_t host_end_pos = resource.find('/');

    // Check if the delimiter is found
    if (host_end_pos == std::string::npos) {
        // Delimiter not found
        return "";
    }

    // Extract the host and URI based on the delimiter position
    std::string host = resource.substr(0, host_end_pos);
    std::string uri = resource.substr(host_end_pos);

    // Default headers
    std::string headers =
        "Host: " + host + "\r\n"
        "User-Agent: MyProxyClient/1.0\r\n"
        "Connection: close\r\n";

    // Construct the GET request string
    std::string getRequest =
        "GET " + uri + " HTTP/1.1\r\n" +
        headers +
        "\r\n";

    return getRequest;
}