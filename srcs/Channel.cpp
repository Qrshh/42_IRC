#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string &name) : channelName(name) {}

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



void Channel::addMember(Client* client) {
    if (std::find(channelMembers.begin(), channelMembers.end(), client) == channelMembers.end())
        channelMembers.push_back(client);
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
