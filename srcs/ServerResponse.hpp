#ifndef SERVER_RESPONSE_HPP
#define SERVER_RESPONSE_HPP

#include "Message.hpp"
#include <sys/socket.h>

// source: https://datatracker.ietf.org/doc/html/rfc1459#section-6
/*
	001 - 399 Reply codes 
	400 - 500 Error codes

	The server sends Replies 001 to 004 to a&sender 
	upon successful registration

	not all of these replycodes have been implemented yet,
	mostly due to needing more info from server

	TODO: add server info either to client or as a variable to be passed 
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

	/*
		Error Responses
	*/
		//"<nickname> :No such nick/channel"
	ERR_NOSUCHNICK			= 401,
		//"<channel name> :No such channel"
	ERR_NOSUCHCHANNEL		= 403,
		//":No recipient given (<command>)"
    ERR_NORECIPIENT 		= 411,
		//":No text to send"
	ERR_NOTEXTTOSEND 		= 412,
		// "<command> :Unknown command"
	ERR_UNKNOWNCOMMAND 		= 421,
		// ":No nickname given"
		// Returned when a nickname parameter expected for a 
		// command and isn't found.
	ERR_NONICKNAMEGIVEN 	= 431,
		// "<nick> :Erroneous nickname" 
		// Returned for invalid characters in nick
	ERR_ERRONEUSNICKNAME 	= 432,
		// "<nick> :Nickname is already in use"
	ERR_NICKNAMEINUSE 		= 433,
		// "<command> :Not enough parameters"
	ERR_NEEDMOREPARAMS 		= 461,
		//":You may not reregister"
	ERR_ALREADYREGISTERED	= 462,
		// ":You have not registered"
	ERR_NOTREGISTERED 		= 451,
		// ":Password incorrect"
	ERR_PASSWDMISMATCH 		= 464,
		//":Permission Denied- You're not an IRC operator"
	ERR_NOPRIVILEGES 		= 481
};

void		sendResponse(Client &sender, std::string message);

namespace Reply
{
	std::string	welcome(Client &sender);
	std::string motd(Client &sender);
}

namespace Error 
{
	std::string nosuchnick(Client &sender, std::string errNick);
	std::string nosuchchannel(Client &sender, std::string errChannel); //check for username 
	std::string norecipient(Client &sender, std::string cmd);
	std::string notexttosend(Client &sender);
	std::string unknowncommand(Client &sender, std::string errCmd);
	std::string nonicknamegiven();
	std::string erroneousnickname(std::string errNick);
	std::string nicknameinuse(std::string errNick);
	std::string needmoreparams(std::string errCmd);
	std::string alreadyregistered();
	std::string notregistered();
	std::string passwdmismatch();
	std::string nopriveleges(Client &sender);
}

#endif
