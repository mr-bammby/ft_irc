#ifndef COMMS_HPP
# define COMMS_HPP
# include "CommandList.hpp"

#include "Client.hpp"
# include <string>
struct Message
{
	Commands	type;
	std::string	content;
	Client		*sender;
};

class Server;
#include "Server.hpp"

Message createMessage(std::string str, Client *cl);
int	executeCommands(Server &serv);
int	passCommand(Message &attempt);

#endif