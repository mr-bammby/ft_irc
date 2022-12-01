#include "ServerResponse.hpp"

namespace Reply
{
	std::string	welcome(Client *user)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("001");
		params.push_back(user->getNickname());
		params.push_back(":Welcome to the Internet Relay Network");
		params.push_back(user->getNickname() + "!" + \
			user->getUsername() + "@" + "PLACEHOLDER SERVER");
		return(Message(params, &server).buildRawMsg());
	}

	std::string motd(Client *user)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("372");
		params.push_back(user->getNickname());
		params.push_back(": fancy cool ascii image plus rules list");
		return(Message(params, &server).buildRawMsg());
	}
}
namespace Error
{
	std::string norecipient(Client *user, std::string cmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("411 *");
		params.push_back(user->getNickname());
		params.push_back(":No recipient given");
		params.push_back("(" + cmd + ")");
		return(Message(params, &server).buildRawMsg());
	}

	std::string notexttosend(Client *user)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("412 *");
		params.push_back(user->getNickname());
		params.push_back(":No text to send");
		return(Message(params, &server).buildRawMsg());
	}

	std::string unknowncommand(Client *user, std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("421 *");\
		params.push_back(user->getNickname());
		params.push_back(errCmd);
		params.push_back("Unknown command");
		return(Message(params, &server).buildRawMsg());
	}

	std::string erroneousnickname(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("432 *");
		params.push_back(errNick);
		params.push_back(":Erroneous Nickname");
		return(Message(params, &server).buildRawMsg());
	}

	std::string nicknameinuse(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("433 *");
		params.push_back(errNick);
		params.push_back(":Nickname is already in use.");
		return(Message(params, &server).buildRawMsg());
	}

	std::string needmoreparams(std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("461 *");
		params.push_back(errCmd);
		params.push_back(":Not enough parameters");
		return(Message(params, &server).buildRawMsg());
	}
	
	std::string notregistered()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("462 *");
		params.push_back(":You have not registered");
		return(Message(params, &server).buildRawMsg());
	}

	std::string passwdmismatch()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("464 *");
		params.push_back(":Password incorrect");
		return(Message(params, &server).buildRawMsg());
	}

	std::string nopriveleges(Client *user)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("481 *");
		params.push_back(user->getNickname());
		params.push_back(":Permission Denied- You're not an IRC operator");
		return(Message(params, &server).buildRawMsg());
	}
}
