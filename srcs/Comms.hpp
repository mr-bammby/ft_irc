#ifndef COMMS_HPP
# define COMMS_HPP
# include "Commands.hpp"

#include "Client.hpp"
#include "color.h"
# include <string>
// struct Message
// {
// 	Commands	type;
// 	std::string	content;
// 	Client		*sender;
// };

class Server;
#include "Server.hpp"

Message createMessage(std::string str, Client *cl);
int	executeCommands(Server &serv);
int	passCommand(Server &serv, Message &attempt);
int	nickCommand(Server &serv, Message &attempt);
int	userCommand(Server &serv, Message &attempt);
int joinCommand(Server &serv, Message &attempt);

int	removeUserFromChannels(Server &serv, std::string nickname);
#endif