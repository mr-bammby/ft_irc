#include "Channel.hpp"

Channel::Channel(std::string name, Client &c)
{
	this->clients.insert(std::make_pair<int, Client*>(c.getFd(), &c));
	this->chanop = c.getNickname();
}

Channel::Channel(const Channel &c): name(c.name)
{}

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

int	Channel::connect(int fd, Client &c)
{
	if (this->invite_only)
	{
		std::vector<std::string>::iterator it = std::find(invited_users.begin(), invited_users.end(), c.getUsername());
		if (it == invited_users.end())
			return (-1);
	}
	clients.insert(std::pair<int, Client*>(fd, &c));
	return (0);
}

int Channel::disconnect(int fd)
{
	clients.erase(fd);
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
	std::map<int, Client*>::iterator it = clients.begin();
	for (; it != clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			break;
	}
	if (it == clients.end())
			return (-1);
	this->clients.erase(it->first);
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

bool Channel::is_op(Client &c)
{
	if (c.getUsername() == chanop)
		return (true);
	return (false);
}

std::string Channel::get_topic()
{
	return(topic);
}
