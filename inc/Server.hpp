#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <set>
# include <string>
# include <poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <iostream>
# include <sstream>
# include "Client.hpp"
# include "Channel.hpp"
# include "defineMessage.hpp"
#include <bits/stdc++.h>

# define BUFFER_SIZE 512

class Server {
private:
	int _serverFd;
	std::vector<struct pollfd> _pollFds;
	std::vector<Client*> clients;
	std::vector<Channel> _channels;
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
	void sendMessageToChannel(const std::string &channelName, const std::string &message);
	
	void registerPassword(Client *client, const std::string buff);
	void tryRegisterClient(Client* client);

	std::string joinParams(const std::vector<std::string>& params);

	void handleQuit(Client *client);
	void handleCap(Client *client, const std::vector<std::string>& args);
	void handlePrivMessageChannel(Client *client, const std::vector<std::string>& params);
	void handlePrivMessageUser(Client *client, const std::string& target, const std::string& message);
	void handleNick(Client *client, const std::string& newNick);
	void handleUser(Client *client, const std::vector<std::string> &params);
	void handleJoin(Client *client, const std::vector<std::string>& args);
	void handlePing(Client* client, const std::vector<std::string>&args);
	void handleTopic(Client *client, const std::vector<std::string> &args);
	void handleModes(Client *client, const std::vector<std::string> &args);
	void handleInvite(Client* invite, const std::vector<std::string>& args);
	void handleKick(Client* kicker, const std::vector<std::string>& args);
	Client* getClientByFd(int fd);
	Client* getClientByNickname(const std::string& nickname);
	Channel *getChannelByName(const std::string& name);
	
};

#endif
