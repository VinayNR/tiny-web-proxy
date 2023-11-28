#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define SOCKET_FAILURE -1
#define SOCKET_CLOSE 0
#define MAX_BUFFER_LENGTH 4096

class TcpSocket {
    public:
        /*
        * Common socket operations
        */
        static int createSocket();
        static void closeSocket(int);
        static int readSocket(int, char *&);
        static int waitOnSocket(int);
        static int writeSocket(int, const char *, size_t);

        /*
        * Additional steps for a listening server
        */
        static int bindSocket(int, char *);
        static int listenSocket(int, int);
        static int acceptConnection(int);
};