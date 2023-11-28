#include "sockutils.h"

/*
* Creates a simple tcp socket and sets the file descriptor
* Returns -1 if the operation failed
*/
int TcpSocket::createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

/*
* Binds the TCP socket to the local port provided in the argument
* Returns -1 if the operation failed
*/
int TcpSocket::bindSocket(int sockfd, char *port) {
    // set the hints
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // server's own address info
    struct addrinfo *addressinfo = nullptr;
    if (getaddrinfo(NULL, port, &hints, &addressinfo) != 0) {
        std::cerr << "Error with get address info on server" << std::endl;
        return -1;
    }

    return bind(sockfd, addressinfo->ai_addr, addressinfo->ai_addrlen);
}

/*
* Listens for active connections on the socket
* Returns -1 if failed
*/
int TcpSocket::listenSocket(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

/*
* Accepts a connection request on the socket
*/
int TcpSocket::acceptConnection(int sockfd) {
    // store client address during incoming connect requests
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    int new_sock_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);

    std::cout << "Accepted a request from Client IP: " << inet_ntoa(client_addr.sin_addr) << std::endl;

    return new_sock_fd;
}

/*
* Writes data on the socket
* Returns the number of bytes of data written on the socket
*/
int TcpSocket::writeSocket(int sockfd, const char *buffer, size_t buffer_length) {
    std::cout << " ------- Inside Write Socket -------" << std::endl;
    ssize_t bytes_sent, total_sent = 0;
    while (total_sent < buffer_length) {
        bytes_sent = write(sockfd, buffer + total_sent, buffer_length - total_sent);
        if (bytes_sent <= 0) {
            perror("closed");
            return SOCKET_CLOSE;
        }
        total_sent += bytes_sent;
    }
    std::cout << " ------- Sent bytes: " << total_sent << " -------- " << std::endl;
    return total_sent;
}

int TcpSocket::waitOnSocket(int sockfd) {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sockfd, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    return select(sockfd + 1, &readSet, nullptr, nullptr, &timeout);
}

/*
* Reads data on the socket
* Returns the number of bytes of data read, or 0/-1 on failure
*/
int TcpSocket::readSocket(int sockfd, char *& buffer) {
    std::cout << " ------- Inside Read Socket -------" << std::endl;
    int bytes_read = -1;

    buffer = new char[MAX_BUFFER_LENGTH];
    memset(buffer, 0, MAX_BUFFER_LENGTH);

    // wait on the socket to check if data is available on the socket
    int status = waitOnSocket(sockfd);

    if (status <= 0) {
        std::cout << "Socket: " << sockfd << " had no data in the timeout interval with status: " << status << std::endl;
        return status;
    }

    // read from the socket
    bytes_read = read(sockfd, buffer, MAX_BUFFER_LENGTH);

    // std::cout << "Buffer: " << buffer << std::endl;
    std::cout << "Receive done with bytes read: " << bytes_read << std::endl;
    
    return bytes_read;
}


/*
* Closes the TCP socket
*/
void TcpSocket::closeSocket(int sockfd) {
    close(sockfd);
}