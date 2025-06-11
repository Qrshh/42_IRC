#include "Channel.hpp"
#include "Client.hpp"
#include "defineMessage.hpp"

Channel::Channel(const std::string &name) : channelName(name), channelTopic(""), inviteOnly(false), topicRestricted(false), userLimit(0) {}

const std::string &Channel::getChannelName() const {
    return channelName;
}

const std::string &Channel::getChannelTopic() const {
    return channelTopic;
}

void Channel::setChannelTopic(const std::string &newTopic) {
    channelTopic = newTopic;
}

void Channel::channelMessage(const std::vector<std::string>& params, Client *client){
	std::string message;

	for(size_t i = 1; i < params.size(); i++){
		message += params[i];
		if(i != params.size() - 1) //ajouter un espace sauf pour le dernier element
			message += " ";
	}

	for(size_t i = 0; i < channelMembers.size(); i++){
		if(channelMembers[i]->getSocket() != client->getSocket()) //celui qui envoie le message ne le recoit pas (logique)
		{
			//preparation du nessage facon irssi
			std::string ircMessage = ":" + client->getNickname() + "!~" + client->getUsername() + "@localhost PRIVMSG " + this->getChannelName() + " " + message + "\r\n";
			//envoyer le message formate au client
			send(channelMembers[i]->getSocket(), ircMessage.c_str(), ircMessage.length(), 0);
		}
	}
}

void Channel::sendMessage(int fd, const std::string &message){
	send(fd, message.c_str(), message.length(), 0);
}

void Channel::joinedMessage(Client *client) {
	std::string ircMessage = ":" + client->getNickname() + "!~" + client->getUsername() +
		"@localhost JOIN " + this->getChannelName() + "\r\n";

	// Envoie au nouveau membre
	sendMessage(client->getSocket(), ircMessage);

	// Puis aux autres membres
	for (size_t i = 0; i < channelMembers.size(); i++) {
		if (channelMembers[i]->getSocket() != client->getSocket()) {
			sendMessage(channelMembers[i]->getSocket(), ircMessage);
		}
	}
}


void Channel::addMember(Client* client) {
    for (size_t i = 0; i < channelMembers.size(); i++) {
        if (channelMembers[i] == client) {
            std::cout << "Client " << client->getNickname() << " déjà membre, pas d'ajout" << std::endl;
            return;
        }
    }
    std::cout << "Ajout de " << client->getNickname() << " au channel " << getChannelName() << std::endl;
    channelMembers.push_back(client);
	joinedMessage(client);
}


void Channel::removeMember(Client* client) {
    channelMembers.erase(std::remove(channelMembers.begin(), channelMembers.end(), client), channelMembers.end());
}

void Channel::addOperator(Client* client) {
    if (std::find(channelOperators.begin(), channelOperators.end(), client) == channelOperators.end())
        channelOperators.push_back(client);
}

void Channel::removeOperator(Client* client) {
    channelOperators.erase(std::remove(channelOperators.begin(), channelOperators.end(), client), channelOperators.end());
}

bool Channel::isOperator(Client* client) const {
    return std::find(channelOperators.begin(), channelOperators.end(), client) != channelOperators.end();
}

void Channel::leaveChannel(Client* client) {
    removeMember(client);
    removeOperator(client);
    client->leaveChannel(this); // Ici c’est OK car Client est connu
}

bool Channel::isInvited(Client *client) const {
	return std::find(invitedClients.begin(), invitedClients.end(), client) != invitedClients.end();
}

bool Channel::isMember(Client* client) const {
    for (size_t i = 0; i < channelMembers.size(); ++i) {
        if (channelMembers[i] == client)
            return true;
    }
    return false;
}

int Channel::findOperator(Client *client){
	for(size_t i = 0;  i < channelOperators.size(); i++){
		if(channelOperators[i] == client)
			return 1;
	}
	return 0;
}


void Channel::addInvitedClient(Client* client){
    if (!isInvited(client)) {
        invitedClients.push_back(client);
    }
}

void Channel::removeInvite(Client* client){
	std::vector<Client*>::iterator it = std::find(invitedClients.begin(), invitedClients.end(), client);
	if(it != invitedClients.end())
		invitedClients.erase(it);
	return ;
}

void Channel::topicChange()
{
	for (size_t i = 0; i < channelMembers.size(); i++)
        sendMessage(channelMembers[i]->getSocket(), RPL_TOPICIS(channelMembers[i]->getNickname(), getChannelName(), getChannelTopic()));
}

