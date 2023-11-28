#include "netutils.h"

/*
* Do a dns lookup of the domain and the port/service name and construct the addrinfo object
*/
void getDomainIp(std::string domain, std::string port, struct addrinfo *& serverinfo) {
    // set the hints
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;

    // get server address info structures
    if (getaddrinfo(domain.c_str(), port.c_str(), &hints, &serverinfo) != 0) {
        std::cout << "Error with get addr info" << std::endl;
        serverinfo = nullptr;
    }
}

std::string extractIpString(struct addrinfo *server_info) {
    // Extract and print the server's IP address
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in *serverAddr = (struct sockaddr_in *)server_info->ai_addr;
    inet_ntop(AF_INET, &(serverAddr->sin_addr), ip_str, INET_ADDRSTRLEN);
    return std::string(ip_str);
}