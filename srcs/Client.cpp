#include "Client.hpp"

Client::Client(int _id, int cl_fd)
	: id(_id), client_fd(cl_fd), nickname(), state(LOCKED), username(), realname()
{}

Client::Client() : id(), nickname(), state(LOCKED), username(), realname() {}

// Client::Client(const Client& c) : client_fd(c.client_fd) {}

Client::~Client() {}

int Client::setNickname(std::string name)
{
	if (state == LOCKED || state == UNINITIALIZED)
	{
		return (-8);//ERR_NOTREGISTERED
	}
	nickname = name;
	if (state == INITIALIZED)
	{
		state = SET;
	}
	return (-5);
}

const std::string& Client::getNickname() const
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
	if (name.empty())
	{
		return (-1);//ERR_NEEDMOREPARAMS 
	}
	if (state == LOCKED)
	{
		return (-2);//ERR_NOTREGISTERED
	}

	// if (state == SET || state == INITIALIZED)

	username = name;
	if (state == INITIALIZED)
	{
		return (-3); //ERR_ALREADYREGISTERED
	}
	username = name;
	state = INITIALIZED;
	return (-4);

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
	// std::vector<Message> msgs = getMessages(command, this);
	// std::cout << "In parsing" << msgs << std::endl;

	Message::commandMap = Message::createCommandMap();
	std::vector<Message> msgs = getMessages(command, this);
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

int Client::getFd()
{
	return (client_fd);
}

void	Client::upgradeState()
{
    if (this->state == LOCKED)
        this->state = UNINITIALIZED;
    else if (this->state == UNINITIALIZED)
        this->state = INITIALIZED;
    else if (this->state == INITIALIZED)
        this->state = SET;
}
