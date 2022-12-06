#include "Channel.hpp"

Channel::Channel(std::string name, Client &c):
name(name)
{
	this->clients.insert(std::pair<std::string, Client*>(c.getNickname(), &c));
	this->chanop = c.getNickname();
	this->invite_only = false;
}

// Channel::Channel(const Channel &c): name(c.name)
// {
// 	this->clients.insert(std::make_pair<std::string, Client*>(c.getNickname(), &c));
// 	this->chanop = c.getNickname();
// }

Channel::~Channel()
{}

Channel &Channel::operator=(const Channel &c)
{
	this->name = c.name;
	return (*this);
}

int Channel::broadcast(std::string message, int sender)
{
	std::map<std::string, Client*>::iterator it;
	std::cout << message << std::endl;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		// send message to all Clients depends of the implementation
		if (sender != it->second->getFd())
			send(it->second->getFd(), message.c_str(), message.length(), 0);
	}
	return (0);
}

int	Channel::connect(Client &c)
{
	if (this->invite_only)
	{
		std::vector<std::string>::iterator it = std::find(invited_users.begin(), invited_users.end(), c.getUsername());
		if (it == invited_users.end())
			return (-1);
	}
	std::cout<<"before insert to channel"<<std::endl;
	clients.insert(std::pair<std::string, Client*>(c.getNickname(), &c));
	std::cout<<"after insert to channel"<<std::endl;
	return (0);
}

int Channel::disconnect(Client &c)
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

int	Channel::cmd_topic(std::string top)
{
	this->topic = top;
	return (0);
}

int	Channel::cmd_names(Client& sender)
{
	std::string listUsers = ":<servername> 353 " + sender.getNickname() + " = " + get_name() + " :";
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (is_op(*it->second))
			listUsers += "@";
		listUsers += it->second->getNickname();
		std::map<std::string, Client*>::iterator it1 = it;
		if ((++it1) != clients.end())
			listUsers += " ";
	}
	listUsers += "\r\n";
	send(sender.getFd(), listUsers.c_str(), listUsers.length(), 0);
	std::string endMsg = ":<servername> 366 " + sender.getNickname() + " " + get_name() + " End of /NAMES list. \r\n";
	send(sender.getFd(), endMsg.c_str(), endMsg.length(), 0);

	return (0);
}

int Channel::cmd_who(Client& sender)
{
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		std::string listUsers = ":<servername> 352 " + sender.getNickname() + " " + get_name() + " " + it->second->getUsername() + "localhost <servername> " + it->second->getNickname() + " H";
		if (is_op(*it->second))
			listUsers += "@";
		listUsers += " :0 " + it->second->getRealname() + "\r\n";
		send(sender.getFd(), listUsers.c_str(), listUsers.length(), 0);
	}
	std::string msg = ":<servername> 315 " + sender.getNickname() + " " + get_name() + " :End of /WHO list\r\n";
	send(sender.getFd(), msg.c_str(), msg.length(), 0);
	return (0);
}

bool Channel::is_op(Client &c)
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

std::string	Channel::get_name()
{
	return (name);
}


bool Channel::is_member(std::string nickname)
{
	std::map<std::string, Client*>::iterator itr = clients.find(nickname);
	if (itr != clients.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::is_member(Client &c)
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
	if (itr != clients.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::can_invite(Client &c)
{
	return (can_invite(c.getNickname()));
}
