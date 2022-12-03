#include "Comms.hpp"

int	executeCommands(Server &serv)
{
	Message *current;
	int	(*Table[])(Server &s, Message &a) =
	{
		&passCommand, &nickCommand, &userCommand, &joinCommand
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
		std::cout << "Executing: " << current->getCommand() << " with: " << current->getParams()[0] << std::endl;
		Table[current->getType()](serv, *current);
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
	if (attempt.getSender()->getState() != 0)
	{
		std::cout << "Error for double password here" << std::endl;
		return (1);
	}
	if (serv.check_password(attempt.getParams()[0]) == true)
	{
		std::cout << "Success!" << std::endl;
		attempt.getSender()->upgradeState();
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
	std::cout << send(attempt.getSender()->getFd(), test.c_str(), test.length(), MSG_CONFIRM) << std::endl;
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

int joinCommand(Server &serv, Message &attempt)
{
	(void)serv;
	(void) attempt;
	std::vector<std::string> list = attempt.getParams();
	return (0);
}

int	removeUserFromChannels(Server &serv, std::string nickname)
{
	std::vector<Channel>::iterator	iter;

	iter = serv.getChannels().begin();
	while (iter != serv.getChannels().end())
	{

	}
	return (0);
}
