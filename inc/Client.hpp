#pragma once
#include <iostream>
#include <vector>

//pour pouvor faire un vector de channel pour le client 
//faire la classe channel donc
class Channel;

class Client{
    private:
        int Fd;
        std::string nickName;
        std::string userName;
        std::string hostName;
        std::string serverName;
        std::string realName;
        bool connected;
        std::vector<Channel*> channels;

    public:
        Client() {connected = false; }

        void setFd(int fd) {Fd = fd; }
        void setUserName(const std::string &user) {userName = user; }
        void setHostName(const std::string &host) {hostName = host; }
        void setServerName(const std::string &server) {serverName = server; }
        void setRealName(const std::string &real) {realName = real; }
        void setNickName(const std::string &nick) {nickName = nick; }
        void setConnected(bool status) {connected = status; }

        bool isConnected() const {return connected; }

        const std::vector<Channel*>& getChannels() const {return channels; }

        void addChannel(Channel* channel) {channels.push_back(channel); }

        int getFd() const { return Fd; }
        std::string getNickName() const {return nickName; }
        std::string getUserName() const {return userName; }
        std::string getHostName() const {return hostName; }

		void removeChannel(Channel* channel) {
			channels.erase(std::remove(channels.begin(), channels.end(), channel), channels.end());
		}


};
