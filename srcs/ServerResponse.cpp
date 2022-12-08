#include "ServerResponse.hpp"

namespace Reply
{
	std::string	welcome(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("001");
		params.push_back(sender.getNickname());
		params.push_back(":Welcome to the Internet Relay Network");
		params.push_back(sender.getNickname() + "!" + \
			sender.getUsername() + "@" + "PLACEHOLDER SERVER");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string motd(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("375");
		params.push_back(sender.getNickname());
		params.push_back(" :- <servername> Message of the Day -\r\n");
		params.push_back("372");
		params.push_back(sender.getNickname());
		params.push_back(": fancy cool ascii image plus rules list \r\n");
		params.push_back("376");
		params.push_back(sender.getNickname());
		params.push_back(" :End of /MOTD command.");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}
}

namespace Error
{
	std::string nosuchnick(Client &sender, std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("401 *");
		params.push_back(sender.getNickname());
		params.push_back(errNick);
		params.push_back(" :No such nick/channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nosuchchannel(Client &sender, std::string errChannel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("403 *");
		params.push_back(sender.getNickname());
		params.push_back(errChannel);
		params.push_back(" :No such channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string norecipient(Client &sender, std::string cmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("411 *");
		params.push_back(sender.getNickname());
		params.push_back(":No recipient given");
		params.push_back("(" + cmd + ")");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notexttosend(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("412 *");
		params.push_back(sender.getNickname());
		params.push_back(":No text to send");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string unknowncommand(Client &sender, std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("421 *");\
		params.push_back(sender.getNickname());
		params.push_back(errCmd);
		params.push_back("Unknown command");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nonicknamegiven()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("431 *");
		params.push_back(":No nickname given");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string erroneousnickname(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("432 *");
		params.push_back(errNick);
		params.push_back(":Erroneous Nickname");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nicknameinuse(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("433 *");
		params.push_back(errNick);
		params.push_back(":Nickname is already in use.");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string needmoreparams(std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("461 *");
		params.push_back(errCmd);
		params.push_back(":Not enough parameters");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}
	
	std::string alreadyregistered()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("462 *");
		params.push_back(":You may not reregister");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notregistered()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("451 *");
		params.push_back(":You have not registered");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string passwdmismatch()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("464 *");
		params.push_back(":Password incorrect");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nopriveleges(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("481 *");
		params.push_back(sender.getNickname());
		params.push_back(":Permission Denied- You're not an IRC operator");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}
}

void		sendResponse(Client &sender, std::string message)
{
	send(sender.getFd(), message.c_str(), message.length(), 0);
}
