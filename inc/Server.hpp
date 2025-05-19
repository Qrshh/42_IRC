// Server.hpp
#pragma once

#include <string>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include "Channel.hpp"
#include <netinet/in.h>



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

private:
    void setupSocket();
    void acceptNewClient();
    void handleClientMessage(int index);
};

