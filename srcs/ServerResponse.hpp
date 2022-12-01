#ifndef SERVER_RESPONSE_HPP
#define SERVER_RESPONSE_HPP

#include "Message.hpp"

// source: https://datatracker.ietf.org/doc/html/rfc1459#section-6
/*
	001 - 399 Reply codes 
	400 - 500 Error codes

	The server sends Replies 001 to 004 to a user 
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
		//"Welcome to the Internet Relay Network <nick>!<user>@<host>"
	RPL_WELCOME 			= 001,
		//"Your host is <servername>, running version <ver>"
	RPL_YOURHOST 			= 002, 
		//"This server was created <date>"
	RPL_CREATED 			= 003,
		//"<servername> <version> <available user modes> <available channel modes>"
	RPL_MYINFO 				= 004,
		// Message of the day 
		// NOTE: Is read from a file.
	RPL_MOTD 				= 372,

	/*
		Error Responses
	*/
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
		// ":You have not registered"
	ERR_NOTREGISTERED 		= 462,
		// ":Password incorrect"
	ERR_PASSWDMISMATCH 		= 464,
		//":Permission Denied- You're not an IRC operator"
	ERR_NOPRIVILEGES 		= 481
};


namespace Reply
{
	std::string	welcome(Client *user);
	std::string motd(Client *user);
}

namespace Error 
{
	std::string norecipient(Client *user, std::string cmd);
	std::string notexttosend(Client *user);
	std::string unknowncommand(Client *user, std::string errCmd);
	std::string erroneousnickname(std::string errNick);
	std::string nicknameinuse(std::string errNick);
	std::string needmoreparams(std::string errCmd);
	std::string notregistered();
	std::string passwdmismatch();
	std::string nopriveleges(Client *user);
}

#endif
