#include "Channel.hpp"

Channel::Channel(std::string ch_name, Client *c): name(ch_name)
{
	this->clients.insert(std::make_pair<const std::string, Client *>(c->getNickname(), c));
	this->chanop = c->getNickname();
}

Channel::Channel(const Channel &c): name(c.name)
{
	// this->clients.insert(std::make_pair<const std::string, Client *>(c.getNickname(), &c));
	this->clients.insert(c.clients.begin(), c.clients.end());
	this->chanop = c.clients.begin()->first;
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
	std::map<const std::string, Client*>::iterator it;
	std::cout << message << std::endl;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		// send message to all Clients depends of the implementation
	}
	return (0);
}

int	Channel::connect(Client *c)
{
	if (this->invite_only)
	{
		std::vector<std::string>::iterator it = std::find(invited_users.begin(), invited_users.end(), c->getUsername());
		if (it == invited_users.end())
			return (-1);
	}
	clients.insert(std::pair<const std::string, Client *>(c->getNickname(), c));
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

int	Channel::cmd_topic(std::string ch_topic)
{
	this->topic = ch_topic;
	return (0);
}

bool Channel::is_op(const Client &c)
{
	return (is_op(c.getNickname()));
}

bool Channel::is_op(std::string nickname)
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
	std::map<const std::string, Client *>::iterator itr = clients.find(nickname);
	if (itr != clients.end())
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
	std::map<const std::string, Client *>::iterator itr = clients.find(nickname);
	if (itr != clients.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::can_invite(const Client &c)
{
	return (can_invite(c.getNickname()));
}

// int	Channel::removeUser(Client &client)
// {
// 	std::vector<Client>::iterator	first = this->clients.begin();
// 	while (first != this->clients.end())
// 	{
// 		if (*first == client)
// 		{
// 			this->clients.erase(first);
// 			return (0);
// 		}
// 		++first;
// 	}
// 	return (1);
// }