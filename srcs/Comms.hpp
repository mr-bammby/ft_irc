#ifndef COMMS_HPP
# define COMMS_HPP
# include "Commands.hpp"

#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include "Server.hpp"

class Server;

int	passCommand(Server &serv, Message &attempt);
int	nickCommand(Server &serv, Message &attempt);
int	userCommand(Server &serv, Message &attempt);
int	joinCommand(Server &serv, Message &attempt);
int	privmsgCommand(Server &serv, Message &attempt);
int	noticeCommand(Server &serv, Message &attempt);
int inviteCommand(Server &serv, Message &attempt);
int	killCommand(Server &serv, Message &attempt);
int	quitCommand(Server &serv, Message &attempt);
int	partCommand(Server &serv, Message &attempt);
int	kickCommand(Server &serv, Message &attempt);
int	whoCommand(Server &serv, Message &attempt);
int	namesCommand(Server &serv, Message &attempt);
int	topicCommand(Server &serv, Message &attempt);
int	operCommand(Server &serv, Message &attempt);
int	squitCommand(Server &serv, Message &attempt);
int	modeCommand(Server &serv, Message &attempt);
int	listCommand(Server &serv, Message &attempt);

#endif
