#include "Channel.hpp"

Channel::Channel(std::string name, const Client &c):
name(name)
{
	this->clients.insert(std::make_pair<std::string, Client*>(c.getNickname(), &c));
	this->chanop = c.getNickname();
}

Channel::Channel(const Channel &c): name(c.name)
{
	this->clients.insert(std::make_pair<std::string, Client*>(c.getNickname(), &c));
	this->chanop = c.getNickname();
}

Channel::~Channel()
{}

Channel &Channel::operator=(const Channel &c)
{
	this->name = c.name;
	return (*this);
}

int Channel::broadcast(std::string message)
{
	std::map<int, Client*>::iterator it;
	std::cout << message << std::endl;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		// send message to all Clients depends of the implementation
	}
	return (0);
}

int	Channel::connect(const Client &c)
{
	if (this->invite_only)
	{
		std::vector<std::string>::iterator it = std::find(invited_users.begin(), invited_users.end(), c.getUsername());
		if (it == invited_users.end())
			return (-1);
	}
	clients.insert(std::pair<std::string, Client*>(c.getNickname(), &c));
	return (0);
}

int Channel::disconnect(const Client &c)
{
	clients.erase(c.getNickname());
	if (clients.empty())
		return (1);
	return (0);
}

int Channel::disconnect(std::string nickname)
{
	clients.erase(nickname);
	if (clients.empty())
		return (1);
	return (0);
}

// perhaps should be handled by server class
/**
 * @brief return fd of the user that should kicked and remove
 * user from clients list
 * 
 * @param nickname 
 * @return int 
 */
int	Channel::cmd_kick(std::string nickname)
{

	this->clients.erase(nickname);
	return (0);
}

int	Channel::cmd_invite(std::string nickname)
{
	this->invited_users.push_back(nickname);
	return (0);
}

int	Channel::cmd_topic(std::string topic)
{
	this->topic = topic;
	return (0);
}

bool Channel::is_op(const Client &c)
{
	return (is_op(c.getNickname));
}

bool Channel::is_op(std::String nickname)
{
	if (nickname == chanop)
		return (true);
	return (false);
}

std::string Channel::get_topic()
{
	return(topic);
}


bool Channel::is_member(std::string nickname)
{
	std::map<std::string, Client*>::iterator itr = clients.find(nickname);
	if (itr != clients_nameMap.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::is_member(const Client &c)
{
	return (is_member(c.getNickname()));
}

/*
Checks if client can invite to this channel
*/
bool Channel::can_invite(std::string nickname)
{
	if (is_op(nickname))
	{
		return (true);
	}
	if (invite_only)
	{
		return (false);
	}
	std::map<std::string, Client*>::iterator itr = clients.find(nickname);
	if (itr != clients_nameMap.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::can_invite(const Client &c)
{
	return (can_invite(c.getNickname()));
}