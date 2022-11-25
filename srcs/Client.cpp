#include "Client.hpp"

Client::Client(int index, int cl_fd)
	: id(index), client_fd(cl_fd), nickname(), state(LOCKED)
{}

Client::Client() : nickname(), state(LOCKED) {}

Client::Client(const Client& c) : client_fd(c.client_fd) {}

Client::~Client() {}

int Client::setNickname(std::string name)
{
	if (state == LOCKED)
	{
		return (-1);
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

int Client::parse(std::string command)
{
	std::vector<Message> msgs = getMessages(command, this);
	std::cout << "In parsing" << msgs << std::endl;

	std::cout << "Command for <" << this->client_fd << "> was: " << command << std::endl;
	return (0);
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
