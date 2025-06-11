#include "Client.hpp"
#include "Channel.hpp"

Client::Client(int socket) : _socket(socket), _registered(false), _sentPass(false), _sentNick(false),
								_sentUser(false), _isOperator(false) {}

Client::~Client(){
	close(_socket);
	for(std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end();++it){
		(*it)->removeMember(this);
	}
	_channels.clear();
	std::cout << "Client " << _socket << " deleted" << std::endl;
}

int Client::getSocket() const {
	return _socket;
}

bool Client::isRegistered() const {
	return _registered;
}

void Client::setRegistered(bool status){
	_registered = status;
}

bool Client::hasSentPass() const {
	return _sentPass;
}

void Client::setSentPass(bool status){
	_sentPass = status;
}

bool Client::hasSentNick() const {
	return _sentNick;
}

void Client::setSentNick(bool status){
	_sentNick = status;
}

bool Client::hasSentUser() const {
	return _sentUser;
}

void Client::setSentUser(bool status){
	_sentUser = status;
}

const std::string& Client::getNickname() const {
	return _nickname;
}

void Client::setNickname(const std::string& nickname){
	_nickname = nickname;
}

const std::string& Client::getHostname() const {
	return _hostname;
}

void Client::setHostname(const std::string& hostname){
	_hostname = hostname;
}

const std::string& Client::getUsername() const {
	return _username;
}

void Client::setUsername(const std::string& username){
	_username = username;
}

const std::string& Client::getServername() const {
	return _servername;
}

void Client::setServername(const std::string& servername){
	_servername = servername;
}

const std::string& Client::getRealname() const {
	return _realname;
}

void Client::setRealname(const std::string& realname){
	_realname = realname;
}

bool Client::isOperator() const {
	return _isOperator;
}

void Client::setOperator(bool status){
	_isOperator = status;
}


void Client::joinChannel(Channel* channel){
	_channels.insert(channel);
}

void Client::leaveChannel(Channel* channel){
	_channels.erase(channel);
}

bool Client::isInChannel(const std::string& channelName) const {
	for(std::set<Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it){
		if((*it)->getChannelName() == channelName)
			return true;
	}
	return false;
}

const std::set<Channel*>& Client::getChannels() const {
	return _channels;
}

std::string& Client::getRecvBuffer(){
    return _recvBuffer;
}

void Client::clearRecvBuffer() {
    _recvBuffer.clear();
}

void Client::eraseRecvBuffer(size_t pos) {
    _recvBuffer.erase(0, pos);
}
