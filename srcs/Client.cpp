#include "Client.hpp"

Client::Client(int index, int cl_fd)
	: id(index), client_fd(cl_fd), nickname(), state(LOCKED), username(), realname()
{}

Client::Client() : nickname(), state(LOCKED), username(), realname() {}

Client::Client(const Client& c) : client_fd(c.client_fd) {}

Client::~Client() {}

int Client::setNickname(std::string name)
{
	if (state == LOCKED)
	{
		return (-1);//ERR_NOTREGISTERED
	}
	nickname = name;
	if (UNINITIALIZED)
	{
		state = INITIALIZED;
	}
	return (0);
}

const std::string& Client::getNickname()
{
	return (nickname);
}

const Client::State& Client::getState()
{
	return (state);
}

const std::string&		Client::getUsername()
{
	return (username);
}

int						Client::setUsername(std::string name)
{
	if (state == LOCKED || state == UNINITIALIZED)
	{
		return (-1);//ERR_NOTREGISTERED
	}
	if (state == SET)
	{
		return (-1); //ERR_ALREADYREGISTERED
	}
	username = name;
	if (state == INITIALIZED)
	{
		state = SET;
		// send RPL_WELCOME message to client(Register connection)
	}
	return (0);
}

const std::string&		Client::getRealname()
{
	return (realname);
}

int						Client::setRealname(std::string name)
{
	if (state == LOCKED || state == UNINITIALIZED)
	{
		return (-1); //ERR_NOTREGISTERED
	}
	if (state == SET)
	{
		return (-1); //ERR_ALREADYREGISTERED
	}
	realname = name;
	return (0);
}

int Client::parse(std::string command)
{
	std::vector<Message> msgs = getMessages(command);
	std::cout << msgs << std::endl;

	std::vector<Message>::iterator it = msgs.begin();
	while (it != msgs.end())
	{
		if (this->state == LOCKED)
			command.compare("PASS");
		std::cout << "Command for <"
					<< "> was: " << command << std::endl;

		// if (it->command == "USER")
		// {
		// 	if (it->params.size() > 0)
		// 		this->setUsername(it->params[0]);
		// 	else
		// 		return (-1); //ERR_NEEDMOREPARAMS
		// }
		it++;
	}
	return (0);
}
