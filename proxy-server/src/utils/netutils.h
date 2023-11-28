#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void getDomainIp(std::string, std::string, struct addrinfo *&);

std::string extractIpString(struct addrinfo *);