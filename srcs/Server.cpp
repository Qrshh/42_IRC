#include "Server.hpp"

Server::Server(int port, const std::string &password) : _password(password) {
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("socket() failed");

	fcntl(_serverFd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind() failed");

	if (listen(_serverFd, SOMAXCONN) < 0)
		throw std::runtime_error("listen() failed");

	struct pollfd pfd;
	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);

	std::cout << "Server listening on port " << port << std::endl;
}

Server::~Server() {
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
	close(_serverFd);
}

void Server::run() {
	while (true) {
		if (poll(&_pollFds[0], _pollFds.size(), -1) < 0) {
			std::cerr << "Poll" << std::endl;
			break;
		}

		for (size_t i = 0; i < _pollFds.size(); ++i) {
			if (_pollFds[i].revents & POLLIN) {
				if (_pollFds[i].fd == _serverFd)
					acceptNewClient();
				else
					handleClientInput(_pollFds[i].fd);
			}
		}
	}
}

void Server::sendMessage(int fd, const std::string& message){
	send(fd, message.c_str(), message.length(), 0);
}

void Server::acceptNewClient() {
	int clientFd = accept(_serverFd, NULL, NULL);
	if (clientFd < 0)
		return;

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	_pollFds.push_back(pfd);
	_clients[clientFd] = new Client(clientFd);

	std::cout << "New client connected: " << clientFd << std::endl;
}

void Server::handleClientInput(int fd) {
	char buffer[BUFFER_SIZE + 1];
	int bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);

	buffer[bytesRead] = '\0';
	std::string data(buffer);

	Client *client = _clients[fd];
	client->getRecvBuffer() += data;

	size_t pos;
	while ((pos = client->getRecvBuffer().find("\r\n")) != std::string::npos) {
		std::string command = client->getRecvBuffer().substr(0, pos);
		client->eraseRecvBuffer(pos + 2);
		std::cout << "Received raw input: [" << data << "]" << std::endl;
		splitCommand(client, command);
	}
}

void Server::splitCommand(Client *client, std::string cmds)
{
    // Enlever les caractères \r et \n à la fin de la chaîne
    std::string input(cmds);
    input.erase(input.find_last_not_of("\r\n") + 1);

    // Séparer les différentes lignes de commande
    std::istringstream stream(input);
    std::string line;

	while (std::getline(stream, line))  // Lire ligne par ligne
	{
		std::string command;
		std::vector<std::string> args;
		// Enlever les espaces supplémentaires en début et fin de ligne
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty()) continue; // Sauter les lignes vides
		// Utiliser un istringstream pour découper la ligne en commande et arguments
		std::istringstream lineStream(line);
		lineStream >> command;  // Extraire la commande (ex: "CAP", "PASS", etc.)

		std::string arg;
		while (lineStream >> arg)
		args.push_back(arg);  // Extraire tous les arguments après la commande
		// Appeler la méthode qui gère la commande
		handleCommand(client, command, args);
	}
}

void Server::handleCommand(Client *client, const std::string &command, std::vector<std::string> args) {
    std::cout << "Command: [" << command << "]" << std::endl;
	for (size_t i = 0; i < args.size(); ++i)
		std::cout << "Arg[" << i << "]: " << args[i] << std::endl;

    if (command == "CAP")
        return;

    // Si client non enregistré, n'accepter que PASS, NICK, USER
    if (!client->isRegistered()) {
        if (command != "PASS" && command != "NICK" && command != "USER") {
            sendMessage(client->getSocket(), "ERROR :You have not registered\r\n");
            return;
        }
    }
    if (command == "PASS") {
        if(client->isRegistered()) {
            sendMessage(client->getSocket(), ERR_ALREADYREGISTERED(client->getNickname()));
            return;
        }
        if(args.empty()) {
            sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
            return;
        }
        this->registerPassword(client, args[0]);
    }
    else if (command == "NICK") {
        if (args.empty()) {
            sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
            return;
        }
        handleNick(client, args[0]);
        tryRegisterClient(client);
        return;
    }
    else if (command == "USER") {
        if (args.size() < 4) {
            sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
            return;
        }
        handleUser(client, args);
        tryRegisterClient(client);
        return;
    }
    else if (command == "QUIT") {
        handleQuit(client);
        return;
    }
    else if (command == "PRIVMSG") {
        if(args.empty()) {
            sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
            return;
        }
        if(args[0][0] == '#'){
            handlePrivMessageChannel(client, args);
        }
		else
		{
			std::string message = joinParams(args);
			handlePrivMessageUser(client, args[0], message);
		}
    }
    else if (command == "JOIN") {
		if (!client->isRegistered()){
			sendMessage(client->getSocket(), ERR_NOTREGISTERED(client->getNickname()));
			return ;
		}
        if (args.empty()) {
            sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
            return;
        }
		std::string channelName = args[0];
		Channel *channel = NULL;
		for(size_t i = 0; i < _channels.size(); i++){
			if(_channels[i].getChannelName() == channelName){
				channel = &_channels[i];
				break;
			}
		}
		if(!channel){
			_channels.push_back(Channel(channelName));
			channel = &_channels.back();
		}
		channel->addMember(client);
		client->setCurrentChannel(channelName);

		std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN :" + channelName + "\r\n";
		sendMessage(client->getSocket(), joinMsg);

		std::string nameList = ":" + std::string("localhost") + " 353 " + client->getNickname() + " = " + channelName + " :" + client->getNickname() + "\r\n";
		sendMessage(client->getSocket(), nameList);

		std::string endNames = ":" + std::string("localhost") + " 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
		sendMessage(client->getSocket(), endNames);
		return ;
	}
}

void Server::registerPassword(Client* client, const std::string buff)
{
	if(buff != _password)
	{
		sendMessage(client->getSocket(), "WRONG PASSWORD !!!!");

		close(client->getSocket());
		delete client;
		return ;
	}
	client->setSentPass(true);
	tryRegisterClient(client);
	return ;
}

void  Server::handleNick(Client *client, const std::string &newNick)
{
	if(newNick.empty() || newNick.length() > 9)
	{
		sendMessage(client->getSocket(), ERR_ERRONEUSNICK(newNick));
		return ;
	}

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if(it->second->getNickname() == newNick)
		{
			sendMessage(client->getSocket(), ERR_NICKINUSE(newNick));
			return ;
		}
	}

	std::string oldNick = client->getNickname();
	client->setNickname(newNick);

	sendMessage(client->getSocket(), RPL_NICKCHANGE(oldNick, newNick));
	client->setSentNick(true);
	return ;
}

void Server::handleUser(Client *client, const std::vector<std::string> &params){
	if(params.size() < 4){
		sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
		return ;
	}
	client->setUsername(params[0]);
	client->setHostname(params[2]);
	client->setServername(params[1]);
	std::string realname;

	for(size_t i = 3; i < params.size(); i++){
		if(i > 3)
			realname += " ";
		realname += params[i];
	}
	if(!realname.empty() && realname[0] == ':')
	 realname = realname.substr(1);
	client->setRealname(realname);

	client->setSentUser(true);
}

void Server::handlePrivMessageChannel(Client *client, const std::vector<std::string>& params){
	for(size_t i = 0; i < _channels.size(); i++)
	{
		std::cout << "-------------entered PRIV MSG \n";
		if(_channels[i].getChannelName() == params[0])
		{
			_channels[i].channelMessage(params, client);
			return ;
		}
	}
}

void Server::handlePrivMessageUser(Client *client, const std::string& target, const std::string& message){
    if(target.empty())
    {
        sendMessage(client->getSocket(), ERR_NORECIPIENT(client->getNickname(), "(PRIVMSG)"));
        return ;
    }
    if(message.empty())
    {
        sendMessage(client->getSocket(), ERR_NOTEXTTOSEND(client->getNickname()));
        return ;
    }

    bool targetFound = false;
    for(std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        std::cout << "Searching ... :" << it->first << "\n";
        if(it->second->getNickname() == target)
        {
            std::cout << "Found\n";
            //Format du message
            std::string formattedMsg = ":" + client->getNickname() + "!" 
                + client->getUsername() + "@" 
                + client->getHostname() 
                + " PRIVMSG " + target 
                + " " + message + "\r\n";

            std::cout << formattedMsg << "\n";
            send(it->second->getSocket(), formattedMsg.c_str(), formattedMsg.length(), 0);
            targetFound = true;
            break ;
        }
    }
    if(!targetFound)
    {
        sendMessage(client->getSocket(), ERR_NOSUCHNICK(target, client->getNickname()));
        return ;
    }

}

void Server::tryRegisterClient(Client* client) {
	if (!_password.empty() && !client->hasSentPass())
		return;

	if (!client->hasSentNick() || !client->hasSentUser())
		return;

	if (!client->isRegistered()) {
		client->setRegistered(true);
		sendMessage(client->getSocket(), RPL_CONNECTED(client->getNickname(), client->getUsername(), client->getHostname()));
	}
}

void Server::handleQuit(Client *client)
{
	std::cout << "Client <" << client->getSocket() << "> Disconnected" << std::endl;
	close(client->getSocket());
	std::cout << "Quit Success "  << std::endl;
	return;
}

std::string Server::joinParams(const std::vector<std::string>& params)
{
	std::string joinedParams;
	for(size_t i = 1; i < params.size(); i++){
		joinedParams += params[i];
		if(i != params.size() - 1)
			joinedParams += " ";
	}
	return joinedParams;
}
