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
		std::cout << RED;
		std::cout << "Executing: " << current->command << " with: " << current->params[0] << std::endl;
		if (current->command == "PASS")
			Table[0](serv, *current);
		std::cout << WH;
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
	if (serv.check_password(attempt.params[0]) == true)
	{
		std::cout << "Success!" << std::endl;
		
	}
	else
		std::cout << "Failed!" << std::endl;
	std::cout << WH;
	return (0);
}
