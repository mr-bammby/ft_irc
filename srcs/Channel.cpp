#include "Channel.hpp"
#include "ServerResponse.hpp"

Channel::Channel(std::string ch_name, Client &c):
name(ch_name)
{
	this->clients.insert(std::pair<std::string, Client*>(c.getNickname(), &c));
	this->owner = c.getNickname();
	this->invite_only = false;
	is_private = false;
	is_secret = false;
	op_topic = false;
	no_msg = false;
	moderated = false;
	user_limit = 10000;
	password = "";
}

Channel::~Channel()
{}

int 	Channel::broadcast(std::string message, int sender)
{
	std::map<std::string, Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (sender != it->second->getFd())
			send(it->second->getFd(), message.c_str(), message.length(), 0);
	}
	return (0);
}

int		Channel::connect(Client &c)
{
	if (this->invite_only)
	{
		std::vector<std::string>::iterator it = std::find(invited_users.begin(), invited_users.end(), c.getNickname());
		if (it == invited_users.end())
			return (-1);
	}
	clients.insert(std::pair<std::string, Client*>(c.getNickname(), &c));
	return (0);
}

int 	Channel::disconnect(Client &c)
{
	clients.erase(c.getNickname());
	if (clients.empty())
		return (1);
	return (0);
}

int 	Channel::disconnect(std::string nickname)
{
	clients.erase(nickname);
	if (clients.empty())
		return (1);
	return (0);
}

int		Channel::client_count()
{
	return (clients.size());
}

int 	Channel::list_coms(Client& sender)
{
	std::string			servername("IRC");
	std::vector<std::string>	params;

	params.push_back("NOTICE");
	params.push_back(this->get_name());
	params.push_back(":The available commands on this channel are TOPIC, PART, NAMES, WHO, PRIVMSG and NOTICE for users");
	std::string			test = Message(CMD_RESPONSE, params, &sender).buildRawMsg();
	send(sender.getFd(), test.c_str(), test.length(), 0);
	params[2] = ":Additional operator commands are MODE, INVITE, KICK.";
	test = Message(CMD_RESPONSE, params, &sender).buildRawMsg();
	send(sender.getFd(), test.c_str(), test.length(), 0);
	return (0);
}

int		Channel::cmd_kick(std::string nickname)
{

	this->clients.erase(nickname);
	return (0);
}

int		Channel::cmd_invite(std::string nickname)
{
	this->invited_users.push_back(nickname);
	return (0);
}

int		Channel::cmd_topic(std::string top)
{
	this->topic = top;
	return (0);
}

bool	Channel::get_invite_only() const
{
	return (invite_only);
}

bool	Channel::get_is_private() const
{
	return (is_private);
}

bool	Channel::get_is_secret() const
{
	return (is_secret);
}

bool	Channel::get_op_topic() const
{
	return (op_topic);
}

bool	Channel::get_no_msg() const
{
	return (no_msg);
}

bool	Channel::get_moderated() const
{
	return (moderated);
}

int		Channel::get_user_limit() const
{
	return (user_limit);
}

std::string					Channel::get_password() const
{
	return (password);
}

std::vector<std::string>	Channel::get_who_speaks_on_moderated() const
{
	return (who_speaks_on_moderated);
}
bool						Channel::can_speak_onchannel(std::string nick)
{
	std::vector<std::string>::iterator it =	std::find(who_speaks_on_moderated.begin(), who_speaks_on_moderated.end(), nick);
	if (it == who_speaks_on_moderated.end() && !is_op(nick))
		return (false);
	return (true);
}

std::vector<std::string>	Channel::get_invited_users() const
{
	return (invited_users);
}

std::vector<std::string>	Channel::get_operators() const
{
	return (operators);
}

int	Channel::add_operator(std::string nick)
{
	operators.push_back(nick);
	return (0);
}

int	Channel::change_operator(std::string sign, std::string nick)
{
	if (sign == "-")
	{
		std::vector<std::string>::iterator	p = std::find(operators.begin(), operators.end(), nick);
		if (p != operators.end())
			operators.erase(p);
	}
	else if (sign == "+")
		operators.push_back(nick);
	return (0);
}

int		Channel::change_is_private(std::string sign)
{
	if (sign == "-")
		is_private = false;
	else if (sign == "+")
		is_private = true;
	return (0);
}

int		Channel::change_is_secret(std::string sign)
{
	if (sign == "-")
		is_secret = false;
	else if (sign == "+")
		is_secret = true;
	return (0);
}

int		Channel::change_optopic(std::string sign)
{
	if (sign == "-")
		op_topic = false;
	else if (sign == "+")
		op_topic = true;
	return (0);
}

int		Channel::change_nomsg(std::string sign)
{
	if (sign == "-")
		no_msg = false;
	else if (sign == "+")
		no_msg = true;
	return (0);
}

int		Channel::change_moderated(std::string sign)
{
	if (sign == "-")
		moderated = false;
	else if (sign == "+")
		moderated = true;
	return (0);
}

int		Channel::change_userlimits(std::string sign, std::size_t limit)
{
	if (sign == "-")
		user_limit = 10000;
	else if (sign == "+")
		user_limit = limit;
	return (0);
}

int		Channel::change_password(std::string sign, std::string key)
{
	if (sign == "-")
		password = "";
	else if (sign == "+")
		password = key;
	return (0);
}

int		Channel::change_invite(std::string sign)
{
	if (sign == "-")
		invite_only = false;
	else if (sign == "+")
		invite_only = true;
	return (0);
}

int		Channel::change_who_speaks_on_moderated(std::string sign, std::string user)
{
	if (sign == "-")
	{
		std::vector<std::string>::iterator	p = std::find(who_speaks_on_moderated.begin(), who_speaks_on_moderated.end(), user);
		if (p != who_speaks_on_moderated.end())
		{
			who_speaks_on_moderated.erase(p);
			return (0);
		}
		return (-1); // dont have this user in list
	}
	else if (sign == "+")
		who_speaks_on_moderated.push_back(user);
	return (0);
}

bool	Channel::limit_full()
{
	if (user_limit > clients.size() || user_limit == 0)
		return (false);
	return (true);
}

bool	Channel::is_invited(std::string nick)
{
	std::vector<std::string>::iterator p = std::find(invited_users.begin(), invited_users.end(), nick);
	if (p == invited_users.end())
		return (false);
	return (true);
}

int		Channel::cmd_names(Client& sender)
{
	if (get_is_secret())
	{
		if (!is_member(sender.getNickname()))
		{
			sendResponse(sender, Error::notonchannel(sender, get_name()));
			return (-4);
		}
	}
	std::string listUsers = ":IRC 353 " + sender.getNickname() + " = " + get_name() + " :";
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (is_op(it->second->getNickname()))
			listUsers += "@";
		listUsers += it->second->getNickname();
		std::map<std::string, Client*>::iterator it1 = it;
		if ((++it1) != clients.end())
			listUsers += " ";
	}
	listUsers += "\r\n";
	send(sender.getFd(), listUsers.c_str(), listUsers.length(), 0);
	std::string endMsg = ":IRC 366 " + sender.getNickname() + " " + get_name() + " End of /NAMES list. \r\n";
	send(sender.getFd(), endMsg.c_str(), endMsg.length(), 0);

	return (0);
}

std::string		Channel::channel_modes()
{
	std::string modes;
	if (operators.size())
		modes += "o";
	if (is_private)
		modes += "p";
	if (is_secret)
		modes += "s";
	if (invite_only)
		modes += "i";
	if (op_topic)
		modes += "t";
	if (no_msg)
		modes += "n";
	if (moderated)
		modes += "m";
	if (user_limit != 10000)
		modes += "l";
	if (who_speaks_on_moderated.size())
		modes += "v";
	if (password != "")
		modes += "k";
	return (modes);
}

int 	Channel::cmd_who(Client& sender)
{
	for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		std::string listUsers = ":IRC 352 " + sender.getNickname() + " " + get_name() + " " + it->second->getUsername() + " localhost IRC " + it->second->getNickname() + " H";
		if (is_op(it->second->getNickname()))
			listUsers += " @";
		listUsers += " :0 " + it->second->getRealname() + "\r\n";
		send(sender.getFd(), listUsers.c_str(), listUsers.length(), 0);
	}
	std::string msg = ":IRC 315 " + sender.getNickname() + " " + get_name() + " :End of /WHO list\r\n";
	send(sender.getFd(), msg.c_str(), msg.length(), 0);
	return (0);
}

bool 	Channel::is_op(std::string nickname)
{
	for (std::vector<std::string>::iterator it = operators.begin(); it != operators.end(); it++)
	{
		if (nickname == *it)
			return (true);
	}
	if (nickname == owner)
		return (true);
	return (false);
}

std::string 	Channel::get_topic()
{
	return(topic);
}

std::string		Channel::get_name() const
{
	return (name);
}

Client			*Channel::get_op() const
{
	std::map<std::string, Client*>::const_iterator itr = clients.find(owner);
	return (itr->second);
}

bool 			Channel::is_member(std::string nickname)
{
	std::map<std::string, Client*>::iterator itr = clients.find(nickname);
	if (itr != clients.end())
	{
		return (true);
	}
	return (false);
}

int 			Channel::can_invite(std::string nickname)
{
	if (is_op(nickname))
	{
		return (0);
	}
	if (invite_only)
	{
		return (-6);
	}
	std::map<std::string, Client*>::iterator itr = clients.find(nickname);
	if (itr != clients.end())
	{
		return (0);
	}
	return (-7);
}
