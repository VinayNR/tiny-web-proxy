#include "server.h"

#include <iostream>
#include <thread>


/*
* Constructs a proxy server
*/
ProxyServer::ProxyServer() {
    // initialization for the listening socket of the proxy server
    sockfd_ = -1;

    // get an instance of the Http Handler
    http_handler = HttpHandler::getInstance();

    // get an instance of the HTML parser
    html_parser = HtmlParser::getInstance();

    // get an instance of the Prefetcher
    prefetcher = Prefetcher::getInstance();

    // initialize the queue
    client_requests_queue = new Queue<int>;
    prefetcher_queue = new Queue<std::string>;

    // set the cache
    http_handler->setDnsCache(&dns_cache_);
    http_handler->setHttpCache(&http_cache_);

    // create a thread pool of worker threads for the proxy server
    client_thread_pool.init(20);
    
    // Create the lambda function for the proxy server worker's task
    auto client_handling_task = [this]() {
        this->processClientRequests(client_requests_queue, http_handler);
    };

    client_thread_pool.createWorkerThreads(client_handling_task);

    // create a thread pool of worker threads for the prefetcher
    prefetcher_thread_pool.init(10);

    // Create the lambda function for the prefetcher worker's task
    auto prefetcher_handling_task = [this]() {
        this->prefetcher->prefetch(prefetcher_queue, http_handler);
    };

    prefetcher_thread_pool.createWorkerThreads(prefetcher_handling_task);
}

/*
* Sets up a TCP socket that listens on the given port
* Socket options can be optionally set
* Exits on a failure
*/
int ProxyServer::setupSocket(char *port) {
    std::cout << "---------- Setting up proxy server ----------" << std::endl;

    // create a tcp socket
    if ((sockfd_ = TcpSocket::createSocket()) == -1) {
        std::cerr << "Socket creation failed for proxy server" << std::endl;
        exit(1);
    }

    // display the socket
    std::cout << "Socket FD: " << sockfd_ << std::endl;

    /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    int optval = 1;
    setsockopt(sockfd_, SOL_SOCKET, optval, (const void *)optval , sizeof(int));

    // bind the socket to the port
    if (TcpSocket::bindSocket(sockfd_, port) == -1) {
        std::cerr << "Socket failed to bind to the local port" << std::endl;
        exit(1);
    }

    // listen on the socket for incoming connections
    if (TcpSocket::listenSocket(sockfd_, SERVER_BACKLOG) == -1) {
        std::cerr << "Socket failed to listen for incoming connections" << std::endl;
        exit(1);
    }
    
    std::cout << " -------- Proxy Server setup complete -------- " << std::endl;

    return 0;
}

/*
* Starts the proxy server
* Actively listens for incoming client connections
*/
void ProxyServer::start() {
    std::cout << std::endl << " ---------- Starting Proxy Server ---------- " << std::endl;

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = TcpSocket::acceptConnection(sockfd_);

        // add the connection received to the queue
        client_requests_queue->enqueue(client_sockfd);

        std::cout << "Added client connection to queue: " << client_sockfd << std::endl;
    }
}

/*
* Process client requests
*/
void ProxyServer::processClientRequests(Queue<int> * client_requests_queue, HttpHandler * http_handler) {
    std::cout << " ------ Proxy Server Thread Pool: " << std::this_thread::get_id() << " ------ " << std::endl;

    while (true) {
        // take a connection from the queue
        int client_sockfd = client_requests_queue->dequeue();
        
        // if the client socket is valid
        if (client_sockfd != -1) {
            std::cout << std::this_thread::get_id() << " obtained a client socket: " << client_sockfd << std::endl;

            // read the client request
            HttpRequest * http_request = http_handler->readRequest(client_sockfd);

            if (http_request != nullptr) {
                std::cout << std::endl << " ----------- The request was success ----------- " <<std::endl;
                // handle the client request
                HttpResponse * http_response;
                if ((http_response = http_handler->handleRequest(http_request)) == nullptr) {
                    std::cerr << "Failed to generate response" << std::endl;
                    exit(1);
                }

                // check if there are links to prefetch if html is returned
                if (http_response->getHeaderValue("Content-Type").substr(0, 9) == "text/html") {
                    std::cout << std::endl << " ----------- The response is html ----------- " <<std::endl;
                    // get all the URI elements from the html string
                    std::vector<UriElement> all_uri_elements = html_parser->getAllUriElements(http_response);

                    // check if each of those links should be prefetched
                    for (auto element: all_uri_elements) {
                        if (prefetcher->shouldPrefetch(element.uri)) {
                            // obtain relative path for the URI
                            std::string resource = http_request->getHeaderValue("Host") + "/" + element.uri;
                            std::cout << " ------- Prefetcher running for: " << resource << " ------- " << std::endl;
                            prefetcher_queue->enqueue(resource);
                        }
                    }
                }

                // send the response to client
                http_handler->writeResponse(client_sockfd, http_response);

                // clean up
                delete http_request;
                delete http_response;
            }
            // close the socket
            TcpSocket::closeSocket(client_sockfd);

            std::cout << std::endl << " ----------- Finished serving the client ----------- " <<std::endl << std::endl;
        }
    }
}

/*
* Shuts down the Proxy server by gracefully
*/
void ProxyServer::shutdown() {
    // wait for the worker threads to complete
    client_thread_pool.joinWorkerThreads();
    prefetcher_thread_pool.joinWorkerThreads();

    // close the TCP socket
    TcpSocket::closeSocket(sockfd_);
}

int main(int argc, char * argv[]) {

    // server command line arguments
    if (argc != 2) {
        std::cerr << "Incorrect number of arguments. Usage: <executable> <port>" << std::endl;
        exit(1);
    }

    // create the main instance of Proxy server
    ProxyServer proxy_server;

    // set up the proxy server listening socket
    proxy_server.setupSocket(argv[1]);

    // start the proxy server and listen indefinitely for incoming client connections
    proxy_server.start();

    // shutdown the proxy server
    proxy_server.shutdown();

    return 0;
}