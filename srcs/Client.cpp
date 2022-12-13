#include "Client.hpp"

Client::Client(int cl_fd)
	: client_fd(cl_fd), nickname(), state(LOCKED), username(), realname(), op(false)
{}

Client::Client() : nickname(), state(LOCKED), username(), realname(), op(false) {}

Client::~Client() {}

int 					Client::setNickname(std::string name)
{
	if (state == LOCKED)
	{
		return (-8);
	}
	if (state == SET || nickname.size() > 0)
		return (-7);
	nickname = name;
	upgradeState();
	return (-5);
}

const std::string& 		Client::getNickname()
{
	return (nickname);
}

const Client::State& 	Client::getState()
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
		return (-1);
	}
	if (state == LOCKED)
	{
		return (-2);
	}
	if (state == SET || username.size() > 0)
	{
		return (-3);
	}
	username = name;
	upgradeState();
	return (-4);

}

bool					Client::is_op()
{
	return (op);
}

bool					Client::set_op(std::string pass)
{
	if (pass == "42")
	{
		op = true;
		return (true);
	}
	return (false);
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

int 					Client::getFd()
{
	return (client_fd);
}

void					Client::upgradeState()
{
	if (this->state == LOCKED)
		this->state = UNINITIALIZED;
	else if (this->state == UNINITIALIZED)
		this->state = INITIALIZED;
	else if (this->state == INITIALIZED)
		this->state = SET;
}
