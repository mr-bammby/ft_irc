#include "Comms.hpp"

int	executeCommands(Server &serv)
{
	Message *current;

	while (serv.getBacklogLength() > 0)
	{
		current = serv.getNextMessage();

		std::cout << "Executing: " << current->content << " by: " << current->sender->getFd() << std::endl;
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