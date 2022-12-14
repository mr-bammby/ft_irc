#ifndef SERVER_RESPONSE_HPP
#define SERVER_RESPONSE_HPP

#include "Message.hpp"
#include "Server.hpp"
#include <sys/socket.h>
#include <fstream>

class Server;
/*
	001 - 399 Reply codes 
	400 - 500+ Error codes
	source: https://datatracker.ietf.org/doc/html/rfc1459#section-6
*/
enum ReplyCode
{
	/*
		Command Responses
	*/
		//"Welcome to the Internet Relay Network <nick>!&sender>@<host>"
	RPL_WELCOME 			= 001,
		//"Your host is <servername>, running version <ver>"
	RPL_YOURHOST 			= 002, 
		//"This server was created <date>"
	RPL_CREATED 			= 003,
		//"<servername> <version> <available&sender modes> <available channel modes>"
	RPL_MYINFO 				= 004,
		//"Channel :Users  Name"
	RPL_LISTSTART			= 321,
		//"<channel> <# visible> :<topic>"
	RPL_LIST				= 322,
		//":End of /LIST"
	RPL_LISTEND				= 323,
		//"<channel> <mode> <mode params>"
	RPL_CHANNELMODEIS		= 324,
		//"<channel> :No topic is set"
	RPL_NOTOPIC				= 331,
		//"<channel> :<topic>"
	RPL_TOPIC				= 332,
		//"<channel> <nick>"
	RPL_INVITING			= 341,
		//"<channel> &sender> <host> <server> <nick>
        // <H|G>[*][@|+] :<hopcount> <real name>"
	RPL_WHOREPLY			= 352,
		//"<name> :End of /WHO list"
	RPL_ENDOFWHO			= 315,
		// "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
	RPL_NAMREPLY			= 353,
		// "<channel> :End of /NAMES list"
	RPL_ENDOFNAMES			= 366,
		// Message of the day 
		// NOTE: Is read from a file.
	RPL_MOTD 				= 372,
		// ":You are now an IRC operator"
	RPL_YOUREOPER			= 381,

	/*
		Error Responses
	*/
		//"<nickname> :No such nick/channel"
	ERR_NOSUCHNICK			= 401,
		//"<server name> :No such server"
	ERR_NOSUCHSERVER		= 402,
		//"<channel name> :No such channel"
	ERR_NOSUCHCHANNEL		= 403,
		//"<channel name> :Cannot send to channel"
	ERR_CANNOTSENDTOCHAN	= 404,
		//":No recipient given (<command>)"
    ERR_NORECIPIENT 		= 411,
		//":No text to send"
	ERR_NOTEXTTOSEND 		= 412,
		// "<command> :Unknown command"
	ERR_UNKNOWNCOMMAND 		= 421,
		//":MOTD File is missing"
	ERR_NOMOTD				= 422,
		// ":No nickname given"
		// Returned when a nickname parameter expected for a 
		// command and isn't found.
	ERR_NONICKNAMEGIVEN 	= 431,
		// "<nick> :Erroneous nickname" 
		// Returned for invalid characters in nick
	ERR_ERRONEUSNICKNAME 	= 432,
		// "<nick> :Nickname is already in use"
	ERR_NICKNAMEINUSE 		= 433,
		// "<nick> <channel> :They aren't on that channel"
	ERR_USERNOTINCHANNEL	= 441,
		// "<channel> :You're not on that channel"
	ERR_NOTONCHANNEL		= 442,
		//"<user> <channel> :is already on channel"
	ERR_USERONCHANNEL		= 443,
		// "<command> :Not enough parameters"
	ERR_NEEDMOREPARAMS 		= 461,
		//":You may not reregister"
	ERR_ALREADYREGISTERED	= 462,
		// ":You have not registered"
	ERR_NOTREGISTERED 		= 451,
		// ":Password incorrect"
	ERR_PASSWDMISMATCH 		= 464,
		//"<channel> :Cannot join channel (+l)"
	ERR_CHANNELISFULL		= 471,
		//"<char> :is unknown mode char to me"
	ERR_UNKNOWNMODE			= 472,
		//"<channel> :Cannot join channel (+i)"
	ERR_INVITEONLYCHAN		= 473,
		//"<channel> :Cannot join channel (+k)"
	ERR_BADCHANNELKEY		= 475,
		//":Permission Denied- You're not an IRC operator"
	ERR_NOPRIVILEGES 		= 481,
		// "<channel> :You're not channel operator"
	ERR_CHANOPRIVSNEEDED	= 482,
		//":Cant change mode for other users"
	ERR_USERSDONTMATCH		= 502
};

void		sendResponse(Client &sender, std::string message);

namespace Reply
{
	std::string	welcome(Client &sender, Server &serv);
	std::string liststart(Client&sender);
	std::string listend(Client&sender);
	std::string notopic(Client&sender, std::string channel);
	std::string topic(Client&sender, std::string channel, std::string top);
	std::string inviting(Client &sender, std::string channel, std::string nick);
	std::string motdstart(Client &sender, Server &serv);
	std::string motd(Client &sender);
	std::string endofmotd(Client &sender);
	std::string youreoper(Client &sender);
}

namespace Error 
{
	std::string nosuchnick(Client &sender, std::string errNick);
	std::string nosuchserver(Client &sender, std::string errServ);
	std::string nosuchchannel(Client &sender, std::string errChannel);
	std::string cannotsendtochan(Client &sender, std::string channel);
	std::string norecipient(Client &sender, std::string cmd);
	std::string notexttosend(Client &sender);
	std::string unknowncommand(Client &sender, std::string errCmd);
	std::string nomotd(Client &sender);
	std::string nonicknamegiven();
	std::string erroneousnickname(std::string errNick);
	std::string nicknameinuse(std::string errNick);
	std::string usernotinchannel(Client &sender, std::string recipient, std::string channel);
	std::string notonchannel(Client &sender, std::string channel);
	std::string useronchannel(Client &sender, std::string recipient, std::string channel);
	std::string needmoreparams(std::string errCmd);
	std::string alreadyregistered();
	std::string notregistered();
	std::string passwdmismatch();
	std::string channelisfull(Client &sender, std::string channel);
	std::string unknownmode(Client &sender, std::string errChar);
	std::string inviteonlychan(Client &sender, std::string channel);
	std::string badchannelkey(Client &sender, std::string channel);
	std::string nopriveleges(Client &sender);
	std::string chanoprivsneeded(Client &sender, std::string channel);
	std::string usersdontmatch(Client &sender);
}

#endif
