#include "Comms.hpp"

int	executeCommands(Server &serv)
{
	Message *current;
	int	(*Table[1])(Server &s, Message &a) =
	{
		&passCommand,
	};

	while (serv.getBacklogLength() > 0)
	{
		current = serv.getNextMessage();
		std::cout << "Executing: " << current->content << " by: " << current->sender->getFd() << std::endl;
		Table[0](serv, *current);
		serv.removeLastMessage();
	}
	return (0);
}

Message createMessage(std::string str, Client *cl)
{
	Message msg;
	msg.content = str;
	msg.sender = cl;
	return (msg);
}

int	passCommand(Server &serv, Message &attempt)
{
	if (serv.check_password(attempt.content) == true)
		attempt.sender->upgradeState();
	else
		std::cout << "Wrong pass" << std::endl;
	return (0);
}
