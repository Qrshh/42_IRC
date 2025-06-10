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
	for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		delete *it;
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

Client* Server::getClientByFd(int fd) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getSocket() == fd) {
            return clients[i];
        }
    }
    return NULL;
}

Client* Server::getClientByNickname(const std::string& nickname) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] && clients[i]->getNickname() == nickname) {
            return clients[i];
        }
    }
    return NULL;
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
    
    // Création du client directement dans le vector
    Client *newClient = new Client(clientFd);
    clients.push_back(newClient);

    std::cout << "New client connected: " << clientFd << std::endl;
}

void Server::handleClientInput(int fd) {
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, sizeof(buffer));  // Initialisation explicite
    int bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);

    if (bytesRead <= 0) {
        if (bytesRead < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return;  // Pas de données disponibles pour le moment
        }
        handleQuit(getClientByFd(fd));
        return;
    }

    Client *client = getClientByFd(fd);
    if (!client) return;

    buffer[bytesRead] = '\0';
    client->getRecvBuffer() += buffer;  // Accumule les données

    size_t pos;
    while ((pos = client->getRecvBuffer().find("\n")) != std::string::npos) {
        std::string line = client->getRecvBuffer().substr(0, pos);
        // Nettoie les \r restants
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        client->eraseRecvBuffer(pos + 1);

        if (!line.empty()) {
            std::cout << "Received raw input: [" << line << "]" << std::endl;
            splitCommand(client, line);
        }
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
	{
		handleCap(client, args);
        return;
	}
    else if (command == "PASS") {
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
        handleJoin(client, args);
		return ;
	}
	else if (command == "PING"){
		handlePing(client, args);
		return ;
	}
}

void Server::handlePing(Client* client, const std::vector<std::string>& args){
	if(args.empty()){
		std::cerr << "PING RECEIVED WITHOUT ARGUMENTS\n";
		return ;
	}

	std::string pongReply = "PONG :" + args[0] + "\r\n";
	std::cout << "Sending to " << client->getNickname() << ": " << pongReply;

	ssize_t sent_bytes = send(client->getSocket(), pongReply.c_str(), pongReply.length(), 0);
	if(sent_bytes < 0)
		perror("send failed");
	else
		std::cout << "Sent PONG reply successfully\n";
}

void Server::handleCap(Client* client, const std::vector<std::string>& args) {
	if (args.size() >= 1 && args[0] == "LS") {
		// On indique qu’on ne supporte aucune capacité
		std::string response = "CAP * LS :" CRLF;
		sendMessage(client->getSocket(), response);
	}
	else if (args[0] == "END")
		std::cout << "CAP END recu" << std::endl;
}

void Server::handleJoin(Client* client, const std::vector<std::string>& args){

	std::cout << "Tentative de JOIN par " << client->getNickname() << " dans " << args[0] << std::endl;

	if (!client->isRegistered()) {
		sendMessage(client->getSocket(), ERR_NOTREGISTERED(client->getNickname()));
		return;
	}
	
	if (args.empty() || args[0] == ":" || args[0].empty()) {
        sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
        return;
    }

    std::string channelName = args[0];
    // Vérification du nom du canal (doit commencer par # ou &)
    if (channelName[0] != '#' && channelName[0] != '&') {
        sendMessage(client->getSocket(), ERR_CHANNELNOTFOUND(client->getNickname(), channelName));
        return;
    }

	for(size_t i = 0; i < _channels.size(); i++)
	{
		if(_channels[i].getChannelName() == args[0])
		{
			if(_channels[i].isInviteOnly() && !_channels[i].isInvited(client)){
				sendMessage(client->getSocket(), ERR_INVITEONLYCHAN(client->getNickname(), args[0]));
				return ;
			}
			if(_channels[i].getUserLimit() > 0)
			{
				if(_channels[i].getMembers().size() >= _channels[i].getUserLimit())
				{
					sendMessage(client->getSocket(), ERR_CHANNELISFULL(client->getNickname(), args[0]));
					return ;
				}
			}
			_channels[i].addMember(client);
			// //supprimer l'invitation que le client avait recu
			// 	std::string joinMsg = ":" + client->getNickname() + "!" +
			// 		client->getUsername() + "@" + client->getHostname() +
			// 		" JOIN :" + channelName + "\r\n";
	
			// sendMessageToChannel(channelName, joinMsg);

			return ;
		}
	}
	Channel newChannel(args[0]);
	newChannel.addMember(client);
	newChannel.addOperator(client);
	_channels.push_back(newChannel);
}

void Server::sendMessageToChannel(const std::string &channelName, const std::string &message) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i].getChannelName() == channelName) {
            const std::vector<Client*> &members = _channels[i].getMembers();
            for (size_t j = 0; j < members.size(); ++j) {
                sendMessage(members[j]->getSocket(), message);
            }
            break;
        }
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

	for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if((*it)->getNickname() == newNick)
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

void Server::handlePrivMessageChannel(Client *client, const std::vector<std::string>& params) {
    if (params.size() < 2) {
        sendMessage(client->getSocket(), ERR_NOTENOUGHPARAM(client->getNickname()));
        return;
    }

    std::string targetChannel = params[0];
    std::string message = joinParams(params);  // Tu peux définir joinParams pour concaténer les params à partir d’un index
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i].getChannelName() == targetChannel) {
            if (!_channels[i].isMember(client)) {
                sendMessage(client->getSocket(), ERR_CANNOTSENDTOCHAN(targetChannel));
                return;
            }

            std::string formattedMessage = ":" + client->getNickname() + "!" + client->getUsername() +
                                           "@" + client->getHostname() + " PRIVMSG " + targetChannel + " :" + message + "\r\n";

            const std::vector<Client*>& members = _channels[i].getMembers();
            for (size_t j = 0; j < members.size(); ++j) {
                if (members[j] != client) {
                    sendMessage(members[j]->getSocket(), formattedMessage);
                }
            }
            return;
        }
    }

    sendMessage(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickname(), targetChannel));
}


void Server::handlePrivMessageUser(Client *client, const std::string &target, const std::string &message) {
    if (target.empty()) {
        sendMessage(client->getSocket(), ERR_NORECIPIENT(client->getNickname(), "(PRIVMSG)"));
        return;
    }

    Client *targetClient = getClientByNickname(target);
    if (!targetClient) {
        sendMessage(client->getSocket(), ERR_NOSUCHNICK(target, client->getNickname()));
        return;
    }

    std::string formattedMsg = ":" + client->getNickname() + "!" 
                            + client->getUsername() + "@" 
                            + client->getHostname()  // Utilise le hostname réel
                            + " PRIVMSG " + target 
                            + " :" + message + "\r\n";

    sendMessage(targetClient->getSocket(), formattedMsg);
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

void Server::handleQuit(Client *client) {
    if (!client) return;
    
    std::cout << "Client <" << client->getSocket() << "> Disconnected" << std::endl;
    
    // Retirer le client des channels
    for (size_t i = 0; i < _channels.size(); ++i) {
        _channels[i].removeMember(client);
    }
    
    // Fermer la connexion
    close(client->getSocket());
    
    // Retirer le client de la liste
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getSocket() == client->getSocket()) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
    
    // Retirer le fd de poll
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        if (_pollFds[i].fd == client->getSocket()) {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
}

std::string Server::joinParams(const std::vector<std::string>& args) {
    std::string result;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1)
            result += " ";
        result += args[i];
    }

    // Supprimer le ':' initial s’il existe
    if (!result.empty() && result[0] == ':')
        result = result.substr(1);

    return result;
}

