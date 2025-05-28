#pragma once
#include <vector>
#include <string>
#include <algorithm> // pour std::find et std::remove
# include <sys/socket.h>

class Client; // forward declaration, suffit pour pointeurs

class Channel {
private:
    std::string channelName;
    std::string channelTopic;
    std::vector<Client*> channelMembers;
    std::vector<Client*> channelOperators;

public:
    Channel(const std::string &name);

    const std::string &getChannelName() const;
    const std::string &getChannelTopic() const;
    const std::vector<Client*> &getMembers() const {return channelMembers;}
    void setChannelTopic(const std::string &newTopic);

	void channelMessage(const std::vector<std::string>& params, Client *client);

    void addMember(Client* client);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;

    void leaveChannel(Client* client); // déclaration seulement, pas d'implémentation ici
};
