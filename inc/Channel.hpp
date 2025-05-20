#pragma once
#include <vector>
#include "Client.hpp"


class Client;

class Channel{
	private:
		std::string channelName;
		std::string channelTopic;
		std::vector<Client*> channelMembers;
		std::vector<Client*> channelOperators;

	public:
		Channel(const std::string &name) : channelName(name) {}

		const std::string &getChannelName() const {return channelName; }
		const std::string &getChannelTopic() const {return channelTopic; }
		void setChannelTopic(const std::string &newTopic) {channelTopic = newTopic; }

		void addMember(Client* client) {
			if (std::find(channelMembers.begin(), channelMembers.end(), client) == channelMembers.end())
				channelMembers.push_back(client);
		}

		void removeMember(Client* client) {
			channelMembers.erase(std::remove(channelMembers.begin(), channelMembers.end(), client), channelMembers.end());
		}

		void addOperator(Client* client) {
			if (std::find(channelOperators.begin(), channelOperators.end(), client) == channelOperators.end())
				channelOperators.push_back(client);
		}

		void removeOperator(Client* client) {
			channelOperators.erase(std::remove(channelOperators.begin(), channelOperators.end(), client), channelOperators.end());
		}

		bool isOperator(Client* client) const {
			return std::find(channelOperators.begin(), channelOperators.end(), client) != channelOperators.end();
		}

		void leaveChannel(Client* client) {
			removeMember(client);
			removeOperator(client);
			client->removeChannel(this); // À implémenter côté Client
		}


};