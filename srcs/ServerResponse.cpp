#include "ServerResponse.hpp"

namespace Reply
{
	std::string	welcome(Client &sender, Server &serv)
	{
		(void)serv;
		Client server;
		std::vector<std::string> params;
		params.push_back("001");
		params.push_back(sender.getNickname());
		params.push_back(":Welcome to the Internet Relay Network");
		params.push_back(sender.getNickname() + "!" + \
			sender.getUsername() + "@localhost");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string liststart(Client&sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("321");
		params.push_back(sender.getNickname());
		params.push_back("Channel :Users  Name");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string listend(Client&sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("323");
		params.push_back(sender.getNickname());
		params.push_back(":End of /LIST");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notopic(Client& sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("331");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":No topic is set");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string topic(Client& sender, std::string channel, std::string top)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("332");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":" + top);
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string inviting(Client &sender, std::string channel, std::string nick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("341");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(nick);
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string motdstart(Client &sender, Server &serv)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("375");
		params.push_back(sender.getNickname());
		params.push_back(":- " + serv.get_name());
		params.push_back("Message of the Day -");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string motd(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		Message motd(CMD_RESPONSE, params, &server);
		std::ifstream motdFile;
		std::string motdStr;
		std::string out;

		params.push_back("372");
		params.push_back(sender.getNickname());
		params.push_back(":-");
		motdFile.open("srcs/ircd.motd");
		if (!motdFile.is_open())
			return (Error::nomotd(sender));
		while(getline(motdFile, motdStr))
		{
			params.push_back(motdStr);
			motd.setParams(params);
			out += motd.buildRawMsg();
			params.pop_back();
		}
		motdFile.close();
		return(out);
	}

	std::string endofmotd(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("376");
		params.push_back(sender.getNickname());
		params.push_back(":End of /MOTD command.");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}

	std::string youreoper(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("381");
		params.push_back(sender.getNickname());
		params.push_back(":You are now an IRC operator");
		return(Message(CMD_RESPONSE, params, &server).buildRawMsg());
	}
}

namespace Error
{
	std::string nosuchnick(Client &sender, std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("401");
		params.push_back(sender.getNickname());
		params.push_back(errNick);
		params.push_back(":No such nick/channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nosuchserver(Client &sender, std::string errServ)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("402");
		params.push_back(sender.getNickname());
		params.push_back(errServ);
		params.push_back(":No such server");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nosuchchannel(Client &sender, std::string errChannel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("403");
		params.push_back(sender.getNickname());
		params.push_back(errChannel);
		params.push_back(":No such channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}
	std::string cannotsendtochan(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("404");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":Cannot send to channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string norecipient(Client &sender, std::string cmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("411");
		params.push_back(sender.getNickname());
		params.push_back(":No recipient given");
		params.push_back("(" + cmd + ")");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notexttosend(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("412");
		params.push_back(sender.getNickname());
		params.push_back(":No text to send");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string unknowncommand(Client &sender, std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("421");\
		params.push_back(sender.getNickname());
		params.push_back(errCmd);
		params.push_back("Unknown command");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nomotd(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("422");\
		params.push_back(sender.getNickname());
		params.push_back(":MOTD File is missing");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nonicknamegiven()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("431");
		params.push_back(":No nickname given");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string erroneousnickname(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("432");
		params.push_back(errNick);
		params.push_back(":Erroneous Nickname");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nicknameinuse(std::string errNick)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("433");
		params.push_back(errNick);
		params.push_back(":Nickname is already in use.");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string usernotinchannel(Client &sender, std::string recipient, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("441");
		params.push_back(sender.getNickname());
		params.push_back(recipient + " " + channel + " :They aren't on that channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notonchannel(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("442");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":You're not on that channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string useronchannel(Client &sender, std::string recipient, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("443");
		params.push_back(sender.getNickname());
		params.push_back(recipient + " " + channel + " :is already on channel");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string needmoreparams(std::string errCmd)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("461");
		params.push_back(errCmd);
		params.push_back(":Not enough parameters");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}
	
	std::string alreadyregistered()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("462");
		params.push_back(":You may not reregister");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string notregistered()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("451");
		params.push_back(":You have not registered");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string channelisfull(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("471");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":Cannot join channel (+l)");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string unknownmode(Client &sender, std::string errChar)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("472");
		params.push_back(sender.getNickname());
		params.push_back(errChar);
		params.push_back(":is unknown mode char to me");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string inviteonlychan(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("473");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":Cannot join channel (+i)");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string badchannelkey(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("475");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":Cannot join channel (+k)");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string passwdmismatch()
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("464");
		params.push_back(":Password incorrect");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string nopriveleges(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("481");
		params.push_back(sender.getNickname());
		params.push_back(":Permission Denied- You're not an IRC operator");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string chanoprivsneeded(Client &sender, std::string channel)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("481");
		params.push_back(sender.getNickname());
		params.push_back(channel);
		params.push_back(":You're not channel operator");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}

	std::string usersdontmatch(Client &sender)
	{
		Client server;
		std::vector<std::string> params;
		params.push_back("502");
		params.push_back(sender.getNickname());
		params.push_back(":Cant change mode for other users");
		return(Message(ERROR_RESPONSE, params, &server).buildRawMsg());
	}
}

void		sendResponse(Client &sender, std::string message)
{
	send(sender.getFd(), message.c_str(), message.length(), 0);
}
