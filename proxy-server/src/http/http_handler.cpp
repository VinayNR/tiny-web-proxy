#include "http_handler.h"

#include <fstream>

/*
* Default constructor
*/
HttpHandler::HttpHandler() {
    // add IPs and domains to the denylist vector
    denylist_domains.push_back("httpbin.org");
    denylist_domains.push_back("example.com");
}

/*
* A singleton pattern to prevent creation of multiple instances of the Http Handler class
* Creates a static thread-safe instance and returns it to the caller
*/
HttpHandler* HttpHandler::getInstance() {
    static HttpHandler instance;
    return &instance;
}

void HttpHandler::setHttpCache(Cache<std::string, std::vector<char>> * http_cache) {
    http_cache_ = http_cache;
}

void HttpHandler::setDnsCache(Cache<std::string, std::string> * dns_cache) {
    dns_cache_ = dns_cache;
}

std::vector<char> HttpHandler::readData(int sockfd) {
    std::cout << std::endl << " ----------- Inside Read data ----------- " <<std::endl;

    int buffer_size = 0;
    char *buffer = nullptr;
    std::vector<char> v;

    std::size_t content_length = 0, total_length = 0, header_size = 0;
    
    do {
        if ((buffer_size = TcpSocket::readSocket(sockfd, buffer)) <= 0) {
            std::cout << " ----------- The buffer size is less than 1, breaking ----------- " << std::endl;
            break;
        }

        // find out the content length of the request
        if (content_length == 0) {
            // get the content length before deserialization of the data
            content_length = searchKeyValueFromRawString(buffer, "Content-Length: ", '\r');
            std::cout << " ----- Found content length ------ " << std::endl;
            std::cout << content_length << std::endl;

            // get the total number of bytes until the start of body
            const char* doubleCRLF = strstr(buffer, "\r\n\r\n");
            if (doubleCRLF != nullptr) {
                header_size = doubleCRLF - buffer;
                std::cout << "Header Size: " << header_size << std::endl;
            }

            // check for content_length_pos
            if (content_length == 0) {
                // the request had no content length header, so breaking out
                v.insert(v.end(), buffer, buffer + buffer_size);
                total_length = buffer_size;
                break;
            }
        }

        // copy buffer into data
        v.insert(v.end(), buffer, buffer + buffer_size);
        total_length += buffer_size;

        // check if everything is read from the socket
        if (content_length + header_size <= total_length) {
            break;
        }

        // clear the buffer
        deleteAndNullifyPointer(buffer, true);
    } while (buffer_size > 0);

    // clear the buffer
    deleteAndNullifyPointer(buffer, true);

    if (total_length > 0) {
        std::cout << "Length of data read: " << total_length << std::endl;
        std::cout << " -------- Finished Read Data ------- " << std::endl;
    }

    return v;
}

/*
* Writes data onto the socket
*/
int HttpHandler::writeData(int sockfd, const char * data, size_t length) {
    std::cout << " ------- Writing Data ------- " << std::endl;
    std::cout << data << std::endl;
    std::cout << "Length of data: " << length << std::endl;
    return TcpSocket::writeSocket(sockfd, data, length);
}

/*
* Reads a client request from the socket
*/
HttpRequest * HttpHandler::readRequest(int sockfd) {
    std::cout << std::endl << " ----------- Reading the request ----------- " << std::endl;
    // create a http request object
    HttpRequest *http_request = new HttpRequest;

    std::vector<char> vec_data = readData(sockfd);

    if (vec_data.size() == 0) return nullptr;
    
    http_request->setSerializedRequest(vec_data.data(), vec_data.size());

    // deserialize the data into a http request object
    if (http_request->deserialize() == -1) {
        std::cerr << "Failed to deserialize request object" << std::endl;
        delete http_request;
        return nullptr;
    }

    std::cout << "Request: " << std::endl;
    std::cout << http_request->getSerializedRequest() << std::endl;
    std::cout << " --------- Finished reading request --------- " << std::endl;
    return http_request;
}

/*
* Reads the server response on the socket
*/
HttpResponse * HttpHandler::readResponse(int sockfd) {
    std::cout << std::endl << " ----------- Reading the response ----------- " << std::endl;
    // create a http response object
    HttpResponse *http_response = new HttpResponse;

    std::vector<char> vec_data = readData(sockfd);

    if (vec_data.size() == 0) return nullptr;

    http_response->setSerializedResponse(vec_data.data(), vec_data.size());

    // deserialize the data into a http response object
    if (http_response->deserialize() == -1) {
        std::cerr << "Failed to deserialize response object" << std::endl;
        delete http_response;
        return nullptr;
    }

    // write the body of the response to a file
    const char* filePath = "output.png";

    // Open the file for writing
    std::ofstream outputFile(filePath, std::ios::binary);

    // Check if the file is successfully opened
    if (outputFile.is_open()) {
        // Write the char* to the file
        outputFile.write(http_response->getHttpResponseBody(), http_response->getHttpResponseBodyLength());

        // Close the file
        outputFile.close();

        std::cout << "Data written to file: " << filePath << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
    }
    
    std::cout << "Response: " << std::endl;
    std::cout << http_response->getSerializedResponse() << std::endl;
    std::cout << " --------- Finished reading response --------- " << std::endl;
    return http_response;
}

/*
* Writes a request to the socket
*/
int HttpHandler::writeRequest(int sockfd, HttpRequest * http_request) {
    std::cout << std::endl << " --------- Writing request --------- " << std::endl;
    return writeData(sockfd, http_request->getSerializedRequest(), http_request->getSerializedRequestLength());
}

/*
* Writes a response to the socket
*/
int HttpHandler::writeResponse(int sockfd, HttpResponse * http_response) {
    std::cout << std::endl << " --------- Writing response --------- " << std::endl;
    return writeData(sockfd, http_response->getSerializedResponse(), http_response->getSerializedResponseLength());
}

/*
* Checks if the http request is for a blocked site
*/
bool HttpHandler::isBlockedSite(HttpRequest * http_request) {
    std::cout << http_request->getHeaderValue("Host") << std::endl;
    std::cout << "Checking if site is blocked" << std::endl;
    for (auto domain: denylist_domains) {
        std::cout << domain << std::endl;
        if (http_request->getHeaderValue("Host") == domain) {
            return true;
        }
    }
    return false;
}

/*
* Main logic for handling a request from a client
* Returns a http response object
*/
HttpResponse * HttpHandler::handleRequest(HttpRequest * http_request) {
    std::cout << " --------- Handling client request --------- " << std::endl;

    HttpResponse * http_response = nullptr;

    // validate the request
    if (!isRequestValid(http_request)) {
        std::cerr << "Invalid Request" << std::endl;
        http_response = new HttpResponse;
        http_response->setHttpStatusCode(400)
                    ->setHttpStatusMessage("Bad Request")
                    ->setHttpVersion(http_request->getHttpVersion())
                    ->addResponseHeaders("Content-Type", "text/plain")
                    ->addResponseHeaders("Content-Length", "11")
                    ->addResponseHeaders("Connection", "close")
                    ->setHttpResponseBody(
                                "Bad Request", 11
                            )
                    ->serialize();
        return http_response;
    }

    // check the request in denylist
    if (isBlockedSite(http_request)) {
        http_response = new HttpResponse;
        http_response->setHttpStatusCode(403)
                    ->setHttpStatusMessage("Forbidden")
                    ->setHttpVersion(http_request->getHttpVersion())
                    ->addResponseHeaders("Content-Type", "text/plain")
                    ->addResponseHeaders("Content-Length", "9")
                    ->addResponseHeaders("Connection", "close")
                    ->setHttpResponseBody(
                                "Forbidden", 9
                            )
                    ->serialize();
        return http_response;
    }

    // check if path is admin proxy
    if (isAdminRequest(http_request)) {
        std::cout << std::endl << " ----------- Admin Request ----------- " <<std::endl;
        // handle admin utility
    }

    // obtain the ip address of the server
    HttpServerAddr remote_addr = getRemoteAddress(http_request);

    // check if the response is cached
    
    std::string cache_key = http_request->getHeaderValue("Host") + http_request->getHttpRequestUri();
    std::vector<char> response_string = http_cache_->get(cache_key);
    if (response_string.size() != 0) {
        // read from the http cache
        std::cout << std::endl << " ----------- Response found in cache ----------- " <<std::endl;
        http_response = new HttpResponse;
        http_response->setSerializedResponse(response_string.data(), response_string.size());
        http_response->deserialize();
        return http_response;
    }

    std::cout << std::endl << " ----------- Response not found in cache ----------- " <<std::endl;

    // create a new TCP socket to open a connection to the server
    int new_sockfd;
    if ((new_sockfd = TcpSocket::createSocket()) == -1) {
        std::cerr << "Failed to create a socket" << std::endl;
        close(new_sockfd);
        return nullptr;
    }

    // check if the domain name is cached
    std::string remote_ip;
    if ((remote_ip = dns_cache_->get(remote_addr.domain_name)) != "") {
        std::cout << std::endl << " ----------- IP found in cache ----------- " <<std::endl;

        // set the remote ip address
        remote_addr.domain_ip = remote_ip;

        // create the sock addr structure
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(std::stoi(remote_addr.port));
        inet_pton(AF_INET, remote_ip.c_str(), &server_addr.sin_addr);

        // connect to the remote server
        if (connect(new_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Error connecting to server\n";
            return nullptr;
        }
    }
    else {
        std::cout << std::endl << " ----------- IP not found in cache ----------- " <<std::endl;
        // do a dns lookup
        struct addrinfo *server_info = nullptr;
        getDomainIp(remote_addr.domain_name, remote_addr.port, server_info);

        // check if the DNS lookup failed
        if (server_info == nullptr) {
            std::cerr << "Failed to obtain IP of the server" << std::endl;
            close(new_sockfd);
            return nullptr;
        }

        // connect to the remote server
        if (connect(new_sockfd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
            std::cerr << "Failed to connect to server" << std::endl;
            close(new_sockfd);
            return nullptr;
        }

        // get readable ip string
        std::string remote_ip_str = extractIpString(server_info);

        // add to the cache
        dns_cache_->set(remote_addr.domain_name, remote_ip_str);
    }

    // send request to the server on behalf of the client
    std::cout << std::endl << " ----------- Making a request to server ----------- " << std::endl;
    if (writeRequest(new_sockfd, http_request) <= 0) {
        std::cerr << "Error writing request on the new socket" << std::endl;
        return nullptr;
    }

    // receive response
    std::cout << std::endl << " ----------- Expecting a response from server ----------- " << std::endl;
    if ((http_response = readResponse(new_sockfd)) == nullptr) {

        // 404 response
        http_response = new HttpResponse;
        http_response->setHttpStatusCode(404)
                    ->setHttpStatusMessage("Not found")
                    ->setHttpVersion(http_request->getHttpVersion())
                    ->addResponseHeaders("Connection", "close")
                    ->serialize();
        
        return http_response;
    }

    // add the connection close header
    http_response->addResponseHeaders("Proxy-Connection", "close");

    // serialize the response
    http_response->serialize();

    // add the response to the http cache
    std::cout << std::endl << " ----------- Added the response to the cache ----------- " << std::endl;
    std::cout << "Cache Key: " << http_request->getHeaderValue("Host") + http_request->getHttpRequestUri() << std::endl;
    std::vector<char> response_vec(http_response->getSerializedResponse(), http_response->getSerializedResponse() + http_response->getSerializedResponseLength());
    http_cache_->set(http_request->getHeaderValue("Host") + http_request->getHttpRequestUri(), response_vec);
    
    // return the response obtained from the server
    return http_response;
}

bool HttpHandler::isRequestValid(HttpRequest *http_request) {
    // the request is invalid if the request is not a GET
    return http_request->getHttpRequestMethod() == "GET";
}

bool HttpHandler::isAdminRequest(HttpRequest *http_request) {
    return false;
}

HttpServerAddr HttpHandler::getRemoteAddress(HttpRequest *http_request) {
    std::string domain = http_request->getHeaderValue("Host"), port = "80";
    size_t hostname_separator = domain.find(':');
    if (hostname_separator != std::string::npos) {
        port = domain.substr(hostname_separator + 1);
        domain = domain.substr(0, hostname_separator);
    }

    return HttpServerAddr{domain, port, ""};
}