// Server.hpp
#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <algorithm>
#include <cctype> 
#include "Client.hpp"
#include "Channel.hpp"


#define IPV4 AF_INET  
#define GLOB_SOCK_OPT SOL_SOCKET
#define TCP SOCK_STREAM
#define CTRL_C SIGINT
#define CTRL_Z SIGTSTP

class Client;

class Channel;

class Server {
private:
    int serverSocket;
    int port;
    std::string password;
    std::vector<struct pollfd> pollFds;
    bool isRunning;


public:
    Server::Server(int _port, const std::string &_password);
    static void signalHandler(int signal);
    static Server* instance; 
};

