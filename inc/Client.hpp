#pragma once
#include <iostream>
#include <unistd.h>
#include <set>
#include "Channel.hpp"

class Channel;

class Client{
	private:
		int _socket;

		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _servername;
		std::string _realname;

		bool _registered;
		bool _sentPass;
		bool _sentNick;
		bool _sentUser;

		bool _isOperator;

		std::set<Channel*> _channels;

	public:
		Client(int socket);
		~Client();

		int getSocket() const ;


		/* ENREGISTREMENT */

		bool isRegistered() const ;
		void setRegistered(bool status) ;

		//savoir si le client a utiliser la commande PASS
		bool hasSentPass() const ;
		void setSentPass(bool status) ;

		//savoir si le client a utilise la commande NICK
		bool hasSentNick() const ;
		void setSentNick(bool status) ;

		//savoir si le client a utilise la commande USER
		bool hasSentUser() const ;
		void setSentUser(bool status) ;


		/* IDENTITE */
		const std::string &getNickname() const ;
		void setNickname(const std::string &nickname);

		const std::string &getHostname() const ;
		void setHostname(const std::string &hostname);

		const std::string& getUsername() const ;
		void setUsername(const std::string &username);

		const std::string &getServername() const ;
		void setServername(const std::string &servername);

		const std::string &getRealname() const ;
		void setRealname(const std::string& Realname);

		bool isOperator() const ;
		void setOperator(bool status); 



		/*Gestion des channels */

		void joinChannel(Channel* channel);

		void leaveChannel(Channel* channel);

		bool isInChannel(const std::string& channelName) const ;

		const std::set<Channel*>& getChannels() const ;
};