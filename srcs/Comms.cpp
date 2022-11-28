#include "Comms.hpp"

int	executeCommands(Server &serv)
{
	Message *current;
	int	(*Table[])(Server &s, Message &a) =
	{
		&passCommand, &nickCommand, &userCommand
	};
	// TODO: get the actual's message type
	// const char *ComList[] =
	// {
	// 	"PASS", "NICK", "USER", "JOIN",
	// 	"PRIVMSG", "NOTICE", "KICK", "MODE",
	// 	"INVITE", "TOPIC", "KILL", "RESTART", NULL};
	while (serv.getBacklogLength() > 0)
	{
		current = serv.getNextMessage();
		std::cout << RED;
		std::cout << "Executing: " << current->command << " with: " << current->params[0] << std::endl;
		Table[current->category](serv, *current);
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
	if (attempt.sender->getState() != 0)
	{
		std::cout << "Error for double password here" << std::endl;
		return (1);
	}
	if (serv.check_password(attempt.params[0]) == true)
	{
		std::cout << "Success!" << std::endl;
		attempt.sender->upgradeState();
	}
	else
		std::cout << "Failed!" << std::endl;
	std::cout << BLANK;
	return (0);
}

int	nickCommand(Server &serv, Message &attempt)
{
	(void)serv;
	(void)attempt;
	std::string	test(":localhost 001 enju");
	test.append(" :Welcome to the Internet Relay Network enju!enju@localhost\n");
	std::cout << BL << "How many sent: ";
	std::cout << send(attempt.sender->getFd(), test.c_str(), test.length(), MSG_CONFIRM) << std::endl;
	std::cout << BLANK;
	return (0);
}

int	userCommand(Server &serv, Message &attempt)
{
	(void)serv;
	(void)attempt;
	std::cout << BLANK;
	return (0);
}
