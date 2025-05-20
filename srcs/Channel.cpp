#include "Channel.hpp"
#include "Client.hpp" // inclure la vraie définition

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
