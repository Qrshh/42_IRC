#pragma once

#define CRLF "\r\n"
#define SERVER_NAME "ft_irc"

/*******************************/
/*** REPLIES - Success Codes ***/
/*******************************/

/* Numeric Replies  */
#define RPL_CONNECTED(nickname, username, hostname) (":" SERVER_NAME " 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "!" + username + "@" + hostname + CRLF)
// #define RPL_CHANNELMODES(nickname, channelname, modes) (": 324 " + nickname + " #" + channelname + " " + modes + CRLF)
// #define RPL_CREATIONTIME(nickname, channelname, creationtime) (": 329 " + nickname + " #" + channelname + " " + creationtime + CRLF)
#define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " " + channelname + " :" + topic + CRLF)
// #define RPL_NAMREPLY(nickname, channelname, clientslist) (": 353 " + nickname + " @ #" + channelname + " :" + clientslist + CRLF)
// #define RPL_ENDOFNAMES(nickname, channelname) (": 366 " + nickname + " #" + channelname + " :END of /NAMES list" + CRLF)

// /* Non-Numeric Command Replies */
// #define RPL_JOINMSG(hostname, ipaddress, channelname) (":" + hostname + "@" + ipaddress + " JOIN #" + channelname + CRLF)
#define RPL_NICKCHANGE(oldnickname, nickname) (":" + oldnickname + " NICK " + nickname + CRLF)
// #define RPL_UMODEIS(hostname, channelname, mode, user) (":" + hostname + " MODE " + channelname + " " + mode + " " + user + CRLF)
// #define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + CRLF)
// #define RPL_INVITING(client, target, channel) (": 341 " + client + " " + target + " " + channel + CRLF)
#define ERR_INVITEONLYCHAN(nick, channel) (std::string(":") + SERVER_NAME + " 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n")
#define RPL_INVITE(from, target, channel) (std::string(":") + from + " INVITE " + target + " " + channel + "\r\n")
#define RPL_INVITING(from, target, channel) (std::string(":") + SERVER_NAME + " 341 " + from + " " + target + " " + channel + "\r\n")

/*******************************/
/*** ERROR MESSAGES - Error Codes ***/
/*******************************/

/* 400s - Client Errors */
#define ERR_NOSUCHNICK(channelname, name) (": 401 #" + channelname + " " + name + " :No such nick/channel" + CRLF)
// #define ERR_NOSUCHSERVER(nickname, server) (": 402 " + nickname + " " + server + " :No such server" + CRLF)
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " " + channelname + " :No such channel" + CRLF)
#define ERR_CANNOTSENDTOCHAN(channel) (": 404 #" + channel + " :Cannot send to channel" + CRLF)
#define ERR_NORECIPIENT(nickname, command) (": 411 " + nickname + " :No recipient given (" + command + ")" + CRLF)
#define ERR_NOTEXTTOSEND(nickname) (": 412 " + nickname + " :No text to send" + CRLF)
// #define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + CRLF)
// #define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + CRLF)
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneous nickname" + CRLF)
#define ERR_NICKINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
// #define ERR_NICKCHANGE(oldnickname, nickname) (": 436 " + oldnickname + " NICK " + nickname + CRLF)
#define ERR_USERNOTINCHANNEL(nickname, channelname) (": 441 " + nickname + " " + channelname + " :They aren't on that channel" + CRLF)
#define ERR_USERONCHANNEL(nickname, channel) (": 443 " + nickname + " " + channel + " :User already on channel" + CRLF)
#define ERR_ALREADYONCHANNEL(channel) (": 443 #" + channel + " :User already on channel" + CRLF)
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + CRLF)
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + CRLF)
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + CRLF)
#define ERR_PASSWDMISMATCH(nickname) (": 464 " + nickname + " :Password incorrect" + CRLF)
// #define ERR_YOUREBANNEDCREEP(nickname) (": 465 " + nickname + " :You're banned from the server" + CRLF)
// #define ERR_KEYSET(channelname) (": 467 #" + channelname + " Channel key already set. " + CRLF)
#define ERR_BADCHANNELKEY(nickname, channel) (": 475 " + nickname + " " + channel + " :Cannot join channel (+k)" + CRLF)

/* 400s - Channel/Operator Errors */
#define ERR_UNKNOWNMODE(nickname, channelname, mode) (": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + CRLF)
// #define ERR_NOPRIVILEGES(nickname) (": 481 " + nickname + " :Permission Denied- You're not an IRC operator" + CRLF)
#define ERR_NOTOPERATOR(channelname) (": 482 " + channelname + " :You're not a channel operator" + CRLF)
// #define ERR_RESTRICTED(nickname) (": 483 " + nickname + " :You're restricted from using this server" + CRLF)
// #define ERR_NOOPERHOST(nickname) (": 491 " + nickname + " :No O-lines for your host" + CRLF)

/*  - MODE Errors */
#define ERR_CHANOPRIVSNEEDED(nick, channel) ("482 " + nick + " " + channel + " :You're not channel operator\r\n")
#define ERR_NEEDMOREPARAMS(nick, command) ("461 " + nick + " " + command + " :Not enough parameters\r\n")
#define ERR_NOSUCHCHANNEL(nick, channel) ("403 " + nick + " " + channel + " :No such channel\r\n")
#define ERR_CHANNELISFULL(nick, channel) ("471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")