#include "Comms.hpp"

int	executeCommands(Server &serv)
{
	Message *current;
	int	(*Table[1])(Server &s, Message &a) =
	{
		&passCommand, NULL, &userCommand
	};
	const char *ComList[] =
	{
		"PASS", "NICK", "USER", "JOIN", \
		"PRIVMSG", "NOTICE", "KICK", "MODE", \
		"INVITE", "TOPIC", "KILL", "RESTART", NULL};
	while (serv.getBacklogLength() > 0)
	{
		current = serv.getNextMessage();
		std::cout << RED;
		std::cout << "Executing: " << current->command << " with: " << current->params[0] << std::endl;
		for (int i = 0; ComList[i] != NULL; i++)
		{
			if (current->command == ComList[i])
				Table[i](serv, *current);
		}
		std::cout << BLANK;
		serv.removeLastMessage();
	}
	return (0);
}

// Message createMessage(std::string str, Client *cl)
// {
// 	Message msg;
// 	msg.content = str;
// 	msg.sender = cl;
// 	return (msg);
// }

int	passCommand(Server &serv, Message &attempt)
{
	if (attempt.client->getState() != 0)
	{
		std::cout << "Error for double password here" << std::endl;
		return (1);
	}
	if (serv.check_password(attempt.params[0]) == true)
	{
		std::cout << "Success!" << std::endl;
		attempt.client->upgradeState();
	}
	else
		std::cout << "Failed!" << std::endl;
	std::cout << BLANK;
	return (0);
}
