#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <iostream>
# include <sstream>
# include "Client.hpp"
# include "Channel.hpp"

# define BUFFER_SIZE 512

class Server {
private:
	int _serverFd;
	std::vector<struct pollfd> _pollFds;
	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;
	std::string _password;

public:
	Server(int port, const std::string &password);
	~Server();

	void run();
	void acceptNewClient();
	void handleClientInput(int fd);
	void splitCommand(Client *client, std::string cmds);
	void handleCommand(Client *client, const std::string &command, std::vector<std::string>args);

	void sendMessage(int fd, const std::string& message);
	
	void registerPassword(Client *client, const std::string buff);

	void handleQuit(Client *client);
};

#endif
