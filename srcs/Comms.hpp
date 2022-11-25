#ifndef COMMS_HPP
# define COMMS_HPP
# include "CommandList.hpp"

#include "Client.hpp"
# include <string>
struct MessageStruct
{
	Commands	type;
	std::string	content;
	Client		*sender;
};

class Server;
#include "Server.hpp"

MessageStruct createMessage(std::string str, Client *cl);
int	executeCommands(Server &serv);
int	passCommand(Server &serv, MessageStruct &attempt);

#endif