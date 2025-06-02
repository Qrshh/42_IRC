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
	bool inviteOnly;
	unsigned long userLimit;
    std::vector<Client*> channelMembers;
    std::vector<Client*> channelOperators;
	std::vector<Client*> invitedClients;

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
	bool isInviteOnly() const {return inviteOnly;}
	bool isInvited(Client *client) const ;
	unsigned long getUserLimit() const {return userLimit;}

	void setUserLimit(int limit) {userLimit = limit;}
	void setInviteOnly(bool mode) {inviteOnly = mode;}

    void leaveChannel(Client* client); // déclaration seulement, pas d'implémentation ici
};
