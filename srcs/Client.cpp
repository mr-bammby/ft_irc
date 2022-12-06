#include "Client.hpp"

Client::Client(int _id, int cl_fd)
	: id(_id), client_fd(cl_fd), nickname(), state(LOCKED), username(), realname()
{}

Client::Client() : id(), nickname(), state(LOCKED), username(), realname() {}

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
	if (name.empty())
	{
		return (-1);//ERR_NEEDMOREPARAMS 
	}
	if (state == LOCKED)
	{
		return (-2);//ERR_NOTREGISTERED
	}
	if (state == INITIALIZED || state == SET)
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
