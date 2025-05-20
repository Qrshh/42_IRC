#include "Server.hpp"

Server::Server(int port, const std::string &password)
    : port(port), password(password), isRunning(true) {
    setupSocket();
}

Server::~Server() {
    for (size_t i = 0; i < pollFds.size(); ++i)
        close(pollFds[i].fd);
}

void Server::setupSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        throw std::runtime_error("Socket creation failed");

    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("Bind failed");

    if (listen(serverSocket, SOMAXCONN) == -1)
        throw std::runtime_error("Listen failed");

    pollfd pfd = {serverSocket, POLLIN, 0};
    pollFds.push_back(pfd);
}

void Server::run() {
    std::cout << "IRC Server running on port " << port << std::endl;

    while (isRunning) {
        if (poll(pollFds.data(), pollFds.size(), -1) == -1)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < pollFds.size(); ++i) {
            if (pollFds[i].revents & POLLIN) {
                if (pollFds[i].fd == serverSocket)
                    acceptNewClient();
                else
                    handleClientMessage(i);
            }
        }
    }
}

void Server::stop() {
    isRunning = false;
}

void Server::acceptNewClient() {
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientFd = accept(serverSocket, (sockaddr *)&clientAddr, &len);
    if (clientFd == -1)
        return;

    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    pollfd newPollFd = {clientFd, POLLIN, 0};
    pollFds.push_back(newPollFd);

    std::cout << "New client connected: fd = " << clientFd << std::endl;
}

void Server::handleClientMessage(int index) {
    char buffer[1024];
    int bytes = recv(pollFds[index].fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        std::cout << "Client disconnected: fd = " << pollFds[index].fd << std::endl;
        close(pollFds[index].fd);
        pollFds.erase(pollFds.begin() + index);
        return;
    }

    buffer[bytes] = '\0';
    std::cout << "Received from fd " << pollFds[index].fd << ": " << buffer;


	std::string input(buffer);

	parseCommand(pollFds[index].fd, input);
}

void Server::parseCommand(int clientFd, const std::string &input){
	(void)clientFd;
	std::istringstream iss(input);
	std::string command;
	iss >> command;

	for(size_t i = 0; i < command.length(); i++)
		command[i] = std::toupper(command[i]);

	if(command == "NICK"){
		std::string nickname;
		iss >> nickname;
		std::cout << "NICK command with nickname: " << nickname << std::endl;
		//associer tout ca au client

	} else if(command == "USER") {
		std::string username, hostname, servername, realname;
		iss >> username >> hostname >> servername;
		std::getline(iss, realname);
		if(!realname.empty() && realname[0] == ':')
			realname = realname.substr(1);
		std::cout << "USER command with username: " << username << ", realname: " << realname << std::endl;
	} else if (command == "PASS") {
		std::string pass;
		iss >> pass;
		std::cout << "PASS command with password: " << pass << std::endl;
	} else {
		std::cout << "Unknow command" << std::endl;
	}
}
