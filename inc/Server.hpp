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
    Server(int port, const std::string &password);
    ~Server();

    void run();
    void stop();
    void setupSocket();
    void acceptNewClient();
    void handleClientMessage(int index);
	void parseCommand(int clientFd, const std::string &input);
};

