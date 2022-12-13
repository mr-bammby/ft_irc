#include "Comms.hpp"
#include <sstream>

int	passCommand(Server &serv, Message &attempt)
{
	if (attempt.getText().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-1);
	}
	std::cout<<"provided pass: "<<attempt.getText()<<std::endl;
	std::cout<<"Status: "<<attempt.getSender()->getState()<<std::endl;
	if (attempt.getSender()->getState() != 0)
	{
		std::cout << "Error for double password here" << std::endl;
		return (1);
	}
	if (serv.check_password(attempt.getParams()[0]) == true)
	{
		std::cout << "Success!" << std::endl;

		attempt.getSender()->upgradeState();
	}
	else
	{
		sendResponse(*(attempt.getSender()), Error::passwdmismatch()); //technically not needed
		std::cout << "Failed!" << std::endl;
	}
	std::cout << BLANK;
	return (0);
}

void introducing(Client *sender, Server &serv)
{
	sendResponse(*sender, Reply::welcome(*sender, serv));
	//sendResponse(*sender, Reply::yourhost(*sender, serv));
	//sendResponse(*sender, Reply::created(*sender));
	//sendResponse(*sender, Reply::myinfo(*sender, serv));
	sendResponse(*sender, Reply::motdstart(*sender, serv));
	sendResponse(*sender, Reply::motd(*sender));
	sendResponse(*sender, Reply::endofmotd(*sender));
}

int	nickCommand(Server &serv, Message &attempt)
{
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-1);
	}
	int res = serv.set_nickName(attempt.getSender(), attempt.getParams()[0]);
	std::cout<<"Res: "<<res<<std::endl;
	std::cout<<std::endl;
	switch(res)
	{
		case -1:
		{
			sendResponse(*(attempt.getSender()), Error::nonicknamegiven());
			break ;
		}
		case -2:
			//client_ptr is not viable pointer
			break;
		case -3:
		{
			sendResponse(*(attempt.getSender()), Error::erroneousnickname(attempt.getParams()[0]));
			break ;
		}
		case -4:
		{
			sendResponse(*(attempt.getSender()), Error::nicknameinuse(attempt.getParams()[0]));
			break ;
		}
		case -5:
		{
			if (attempt.getSender()->getState() == 3)
				introducing(attempt.getSender(), serv);
			break;
		}
		case -6:
		{
			sendResponse(*(attempt.getSender()), Error::nicknameinuse(attempt.getParams()[0]));
			break ; //is this intentionally here twice??
		}
		case -7:
		{
			sendResponse(*(attempt.getSender()), Error::alreadyregistered());
			break;
		}
		case -8:
		{
			sendResponse(*(attempt.getSender()), Error::notregistered());
			break ;
		}
	}

	return (0);
}

int	userCommand(Server &serv, Message &attempt)
{
	(void)serv;
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	int res = attempt.getSender()->setUsername(attempt.getParams()[0]);
	switch (res)
	{
		case -1:
		{
			sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
			break;
		}
		case -2:
		{
			sendResponse(*(attempt.getSender()), Error::notregistered());
			break;
		}
		case -3:
		{
			sendResponse(*(attempt.getSender()), Error::alreadyregistered());
			break;
		}
		case -4:
		if (attempt.getSender()->getState() == 3)
		{
			attempt.getSender()->setRealname(attempt.getText());
			introducing(attempt.getSender(), serv);
		}
			break;
	}

	return (0);
}

int	joinCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	//extracting channels and passwords
	std::vector<std::string> channels;
	std::vector<std::string> passwords;
	channels = split(attempt.getParams()[0], ",");
	if (attempt.getParams().size() > 1)
		passwords = split(attempt.getParams()[1], ",");
	//checking if channel exists, if not it should be created and creator becomes operator of that channel
	std::string currentPass; // will contain current password in while loop, because first password should be used for first channel and so on and so on
	std::size_t i = 0;
	while (i < channels.size())
	{
		if (i < passwords.size())
			currentPass = passwords[i];
		else
			currentPass = "";
		Channel* tmp = serv.get_channelPtr(channels[i]);
		if (tmp == NULL)//channel not exists and creating new
		{
			if (channels[i][0] != '#' && channels[i][0] != '&')
			{
				sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), channels[i]));
				return (-2);
			}
			// creating channel with channels[i] as a name and making client as a operator
			Channel* tmp2 = serv.create_channel(channels[i], *attempt.getSender());
			std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost JOIN" + " :" + tmp2->get_name() + "\r\n";
			tmp2->broadcast(msg, 0);
			tmp2->cmd_names(*attempt.getSender());
			tmp2->list_coms(*attempt.getSender());
			std::cout<<"Channels created"<<std::endl;
		}
		else //channel exists and trying to connect to it
		{
			//connecting to channel by sending channels[i] as a channel name and currentPass as a password for this channel
			// it should be checked if channel is invite only, if channel is full, if client is banned from channel, 
			//if password is correct for channel, if client joined to too many channels

			//if everything ok. Client recives NOTICE about all commands avaliable which affects channel
			//(MODE, KICK, PART, QUIT, PRIVMSG/NOTICE).
			// If a JOIN is successful, the user is then sent the channel's topic
			//(using RPL_TOPIC) and the list of users who are on the channel (using
			//RPL_NAMREPLY), which must include the user joining.
			std::cout<<"Channel exists, name: "<<tmp->get_name()<<std::endl;
			if (tmp->limit_full())
			{
				sendResponse(*(attempt.getSender()), Error::channelisfull(*(attempt.getSender()), channels[i]));
				return (-4);
			}
			if (tmp->get_invite_only())
			{
				if (!tmp->is_invited(attempt.getSender()->getNickname()))
				{
					sendResponse(*(attempt.getSender()), Error::inviteonlychan(*(attempt.getSender()), channels[i]));
					return (-5);
				}
			}
			if (tmp->get_password() != currentPass)
			{
				sendResponse(*(attempt.getSender()), Error::badchannelkey(*(attempt.getSender()), channels[i]));
				return (-6);
			}
			tmp->connect(*attempt.getSender());
			if (tmp->get_op_topic() != false)
				sendResponse(*(attempt.getSender()), Reply::topic(*(attempt.getSender()), tmp->get_name(), tmp->get_topic()));
			else 
				sendResponse(*(attempt.getSender()), Reply::notopic(*(attempt.getSender()), tmp->get_name()));
			std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost JOIN" + " :" + tmp->get_name() + "\r\n";
			tmp->broadcast(msg, 0);
			topicCommand(serv, attempt);
			tmp->cmd_names(*attempt.getSender());
			tmp->list_coms(*attempt.getSender());
		}
		i++;
	}
	return (0);
}

int	privmsgCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::norecipient(*(attempt.getSender()), attempt.getCommand()));
		return (-2);
	}
	if (attempt.getText().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::notexttosend(*(attempt.getSender())));
		return (-2);
	}
	std::vector<std::string> recipients = split(attempt.getParams()[0], ",");
	// iterating thorough all the recipients whom to send message
	std::vector<std::string>::iterator it = recipients.begin();
	while (it != recipients.end())
	{
		// checking if channel or client exists with provided name
		std::cout << "check receiver: " << *it << std::endl;
		Client* tmp = serv.get_clientPtr(*it);
		if (tmp == NULL)
		{
			Channel* tmp2 = serv.get_channelPtr(*it);
			if (tmp2 == NULL)
			{
				sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), *it));
				return (-4);
			}
			else
			{
				//sending to channel
				if (tmp2->get_no_msg() && !tmp2->is_member(attempt.getSender()->getNickname()))
				{
					sendResponse(*(attempt.getSender()), Error::cannotsendtochan(*(attempt.getSender()), tmp2->get_name()));
					return (-5);
				}
				if (tmp2->get_moderated() && !tmp2->can_speak_onchannel(attempt.getSender()->getNickname()))
				{
					sendResponse(*(attempt.getSender()), Error::cannotsendtochan(*(attempt.getSender()), tmp2->get_name()));
					return (-5);
				}
				std::string	message;
				message = ":" + attempt.getSender()->getNickname() + " PRIVMSG " + tmp2->get_name() + " :" + attempt.getText().substr(tmp2->get_name().size() + 1) + "\r\n";
				std::cout<<"Sending message: "<<message<<std::endl;
				tmp2->broadcast(message, attempt.getSender()->getFd());
				// send(tmp->getFd(), message.c_str(), message.length(), 0);
				message.clear();
			}
		}
		else
		{
			std::string	message;
			// message = ":boriss PRIVMSG bobo :aaaa\r\n";
			message = ":" + attempt.getSender()->getNickname() + " PRIVMSG " + tmp->getNickname() + " :" + attempt.getText().substr(tmp->getNickname().size() +1 ) + "\r\n";
			std::cout<<"Sending message: "<<message<<std::endl;
			send(tmp->getFd(), message.c_str(), message.length(), 0);
			message.clear();
			// sending to client
			// if client is away, sending RPL_AWAY message
		}
		
		it++;
	}

	return (0);
}

int	noticeCommand(Server &serv, Message &attempt)
{
	// How I understand, NOTICE is the same as PRIVMSG, it just dont send any replies, even if some errors occur 
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() == 0)
		return (-2); 
	if (attempt.getText().size() == 0)
		return (-3);
	
	std::vector<std::string> recipients = split(attempt.getParams()[0], ",");
	// iterating thorough all the recipients whom to send message
	std::vector<std::string>::iterator it = recipients.begin();
	while (it != recipients.end())
	{
		// checking if channel or client exists with provided name
		Client* tmp = serv.get_clientPtr(*it);
		if (tmp == NULL)
		{
			Channel* tmp2 = serv.get_channelPtr(*it);
			if (tmp2 == NULL)
				return (-4);
			else
			{
				//sending to channel
				if (tmp2->get_no_msg() && !tmp2->is_member(attempt.getSender()->getNickname()))
				{
					sendResponse(*(attempt.getSender()), Error::cannotsendtochan(*(attempt.getSender()), tmp2->get_name()));
					return (-5);
				}
				if (tmp2->get_moderated() && !tmp2->can_speak_onchannel(attempt.getSender()->getNickname()))
				{
					sendResponse(*(attempt.getSender()), Error::cannotsendtochan(*(attempt.getSender()), tmp2->get_name()));
					return (-5);
				} // should these 2 errors even send if notice doesn't send errors?
				std::string	message;
				message = ":" + attempt.getSender()->getNickname() + " NOTICE " + tmp2->get_name() + " :" + attempt.getText().substr(tmp2->get_name().size() + 1) + "\r\n";
				std::cout<<"Sending message: "<<message<<std::endl;
				tmp2->broadcast(message, attempt.getSender()->getFd());
				// send(tmp->getFd(), message.c_str(), message.length(), 0);
				message.clear();
			}
		}
		else
		{
			std::string	message;
			message = ":" + attempt.getSender()->getNickname() + " NOTICE " + tmp->getNickname() + " :" + attempt.getText().substr(tmp->getNickname().size() +1 ) + "\r\n";
			std::cout<<"Sending message: "<<message<<std::endl;
			send(tmp->getFd(), message.c_str(), message.length(), 0);
			message.clear();
			// sending to client
		}
		
		it++;
	}

	return (0);
}

int	inviteCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() < 2)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Channel* chn = serv.get_channelPtr(attempt.getParams()[1]);
	Client* cln = serv.get_clientPtr(attempt.getParams()[0]);
	if (cln == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
 	}
	if (chn == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), attempt.getParams()[1]));
		return (-4);
 	}
	if (!chn->is_member(attempt.getSender()->getNickname()))
	{
		sendResponse(*(attempt.getSender()), Error::notonchannel(*(attempt.getSender()), chn->get_name()));
		return (-5); // this said ERR_USERONCHANNEL, but i think it should be not on channel, change if needed 
	}
	int chn_resp = chn->can_invite(attempt.getSender()->getNickname());
	if (chn_resp == 0)
	{
		chn->cmd_invite(cln->getNickname());
		sendResponse(*(attempt.getSender()), Reply::inviting(*(attempt.getSender()), chn->get_name(), cln->getNickname()));
		std::string msg1 = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost INVITE " + cln->getNickname() + " " + chn->get_name() + "\r\n";
		send(cln->getFd(), msg1.c_str(), msg1.length(), 0); 
		//send invite
	}
	else if (chn_resp == -6)
	{
		sendResponse(*(attempt.getSender()), Error::chanoprivsneeded(*(attempt.getSender()), chn->get_name()));
		return (-6);
 	}
	else if (chn_resp == -7)
	{
		sendResponse(*(attempt.getSender()), Error::notonchannel(*(attempt.getSender()), chn->get_name()));
		return (-7);
	}
	return (0);


}

int	killCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered()); 
		return (-1);
	}
  	if (!attempt.getSender()->is_op())
	{
		sendResponse(*(attempt.getSender()), Error::nopriveleges(*(attempt.getSender())));
		return (-4);
	}
	if (attempt.getParams().empty())
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Client* tmp = serv.get_clientPtr(attempt.getParams()[0]);
	if (tmp == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
	}
	serv.deleteUser(tmp);
	return (0);
}

int	quitCommand(Server &serv, Message &attempt)
{
	// if (attempt.getSender()->getState() != 3)
	// 	return (-1); //sending ERR_NOTREGISTERED
	serv.deleteUser(attempt.getSender());

	return (0);
}

int	partCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	//extracting channels and passwords
	std::vector<std::string> channels = split(attempt.getParams()[0], ",");
	std::size_t i = 0;
	while (i < channels.size())
	{
		Channel* tmp = serv.get_channelPtr(channels[i]);
		if (tmp == NULL)//channel not exists and creating new
		{
			sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), channels[i]));
			return (-3);
		}
		else if (!tmp->is_member(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::notonchannel(*(attempt.getSender()), channels[i]));
			return (-4);
		}
		std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost PART " + channels[i] + "\r\n";
		tmp->broadcast(msg, 0);
		tmp->disconnect(attempt.getSender()->getNickname());
		if (tmp->client_count() == 0)
			serv.deleteChannel(tmp->get_name());
		i++;
	}
	return (0);
}

int	kickCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() < 2)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	std::vector<std::string> channels = split(attempt.getParams()[0], ",");
	std::vector<std::string> users = split(attempt.getParams()[1], ",");
	std::size_t i = 0;
	while (i < channels.size())
	{
		Channel* tmp = serv.get_channelPtr(channels[i]);
		if (tmp == NULL)
		{
			sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), channels[i]));
			return (-3);
		}
		else if (!tmp->is_member(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), channels[i]));
			return (-4);
		}
		else if(!tmp->is_op(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::chanoprivsneeded(*(attempt.getSender()), channels[i]));
			return (-5);
		}
		else if (i >= users.size())
		{
			sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
			return (-6);
		}
		else if (!tmp->is_member(users[i]))
		{
			sendResponse(*(attempt.getSender()), Error::usernotinchannel(*(attempt.getSender()), users[i], channels[i]));
			return (-7);
		}
		std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost KICK " + channels[i] + " " + users[i] + " :" + attempt.getSender()->getNickname() + "\r\n";
		tmp->broadcast(msg, 0);
		tmp->disconnect(users[i]);
		if (tmp->client_count() == 0)
			serv.deleteChannel(tmp->get_name());
		i++;
	}
	return (0);
}

int	whoCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() < 1)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Channel* tmp = serv.get_channelPtr(attempt.getParams()[0]);
	if (tmp == NULL)
  {
		sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
  }
	if (tmp->get_is_private() || tmp->get_is_secret())
	{
		if (!tmp->is_member(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::notonchannel(*(attempt.getSender()),tmp->get_name()));
			return (-4);
		}
	}
	tmp->cmd_who(*attempt.getSender());
	return (0);
}

int	namesCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() == 0)
	{
		serv.cmd_namesAllchannels(*attempt.getSender());
		return (0);
	}
	std::vector<std::string> channels = split(attempt.getParams()[0], ",");
	std::size_t i = 0;
	while (i < channels.size())
	{
		Channel* tmp = serv.get_channelPtr(channels[i]);
		if (tmp != NULL)
			tmp->cmd_names(*attempt.getSender());
		i++;
	}
	return (0);
}

int	topicCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	// 0 Doesn't actually change with just TOPIC input
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Channel* tmp = serv.get_channelPtr(attempt.getParams()[0]);
	if (tmp == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
	}
	else if (!tmp->is_member(attempt.getSender()->getNickname()))
	{
		sendResponse(*(attempt.getSender()), Error::notonchannel(*(attempt.getSender()), attempt.getParams()[0]));
		return (-4);
	}
	if (attempt.getParams().size() < 2)
	{
		if (tmp->get_topic().size() == 0)
		{
			sendResponse(*(attempt.getSender()), Reply::notopic(*(attempt.getSender()), tmp->get_name()));
			return (0);
		}
		std::cout << BL << "Said the topic" << std::endl << BLANK;
		sendResponse(*(attempt.getSender()), Reply::topic(*(attempt.getSender()), tmp->get_name(), tmp->get_topic()));
	}
	else
	{
		std::cout << BL << "Setting topic" << std::endl << BLANK;
		if (tmp->get_op_topic() && !tmp->is_op(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::chanoprivsneeded(*(attempt.getSender()), attempt.getParams()[0]));
			return (-5);
		}
		tmp->cmd_topic(attempt.getParams()[1]);
		std::string msg = Reply::topic(*(attempt.getSender()), tmp->get_name(), tmp->get_topic());
		sendResponse(*(attempt.getSender()), msg);
		tmp->broadcast(msg, attempt.getSender()->getFd());		// Added broadcast so the change is known to other clients in the channel
		return (0);
	}
	return (0);
}

int	operCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() < 2)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Client*	tmp = serv.get_clientPtr(attempt.getParams()[0]);
	if (tmp == NULL || tmp->getNickname() != attempt.getSender()->getNickname())
	{
		sendResponse(*(attempt.getSender()), Error::usersdontmatch(*(attempt.getSender())));
		return (-4);
	}
	if (attempt.getSender()->set_op(attempt.getParams()[1]))
	{
		sendResponse(*(attempt.getSender()), Reply::youreoper(*(attempt.getSender())));
		return (0);
	}
	sendResponse(*(attempt.getSender()), Error::passwdmismatch());
	return (-5);
}

int	squitCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (!attempt.getSender()->is_op())
	{
		sendResponse(*(attempt.getSender()), Error::nopriveleges(*(attempt.getSender())));
		return (-2);
	}
		
	if (attempt.getParams().size() == 0)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-3);
	}
	if (attempt.getParams()[0] != serv.get_name())
	{
		sendResponse(*(attempt.getSender()), Error::nosuchserver(*(attempt.getSender()), attempt.getParams()[0]));
		return (-4);
	}
	serv.on = false;
	return (0);
}

int	minus(Message &attempt, Channel &ch)
{
	std::size_t y = 2;
	std::string tmp = attempt.getParams()[1];
	for (std::size_t i = 1; tmp[i]; i++)
	{
		if (tmp[i] == 'o')
		{
			if (attempt.getParams().size() < (y + 1) || !ch.is_member(attempt.getParams()[y]))
			{
				y++;
				return (-5);
			}
			ch.change_operator("-", attempt.getParams()[y]);
			y++;
		}
		else if (tmp[i] == 'p')
			ch.change_is_private("-");
		else if (tmp[i] == 's')
			ch.change_is_secret("-");
		else if (tmp[i] == 'i')
			ch.change_invite("-");
		else if (tmp[i] == 't')
			ch.change_optopic("-");
		else if (tmp[i] == 'n')
			ch.change_nomsg("-");
		else if (tmp[i] == 'm')
			ch.change_moderated("-");
		else if (tmp[i] == 'l')
		{
			ch.change_userlimits("-", 10000);
			y++;
		}
		else if (tmp[i] == 'v')
		{
			if (attempt.getParams().size() < (y + 1))
				return (-6);
			ch.change_who_speaks_on_moderated("-", attempt.getParams()[y]);
			y++;
		}
		else if (tmp[i] == 'k')
		{
			if (attempt.getParams().size() < (y + 1))
				return (-6);
			ch.change_password("-", attempt.getParams()[y]);
		}
		else
			return (-7);
	}
	return (0);
}

int	plus(Message &attempt, Channel &ch)
{
	std::cout<<"plus"<<std::endl;
	std::size_t y = 2;
	std::string tmp = attempt.getParams()[1];
	for (std::size_t i = 1; tmp[i]; i++)
	{
		if (tmp[i] == 'o')
		{
			if (attempt.getParams().size() < (y + 1))
			{
				y++;
				return (-5);
			}
			ch.change_operator("+", attempt.getParams()[y]);
			y++;
		}
		else if (tmp[i] == 'p')
			ch.change_is_private("+");
		else if (tmp[i] == 's')
			ch.change_is_secret("+");
		else if (tmp[i] == 'i')
			ch.change_invite("+");
		else if (tmp[i] == 't')
			ch.change_optopic("+");
		else if (tmp[i] == 'n')
			ch.change_nomsg("+");
		else if (tmp[i] == 'm')
			ch.change_moderated("+");
		else if (tmp[i] == 'l')
		{
			if (attempt.getParams().size() < (y + 1))
				return (-6);
			const char* argv = attempt.getParams()[y].c_str();
			for (int i = 0; argv[i]; i++)
			{
				if (std::isdigit(argv[i]) == 0)
					return (-6);
			}
			std::size_t limit;
			sscanf(argv, "%lu", &limit);
			ch.change_userlimits("+", limit);
			y++;
		}
		else if (tmp[i] == 'v')
		{
			if (attempt.getParams().size() < (y + 1))
				return (-6);
			ch.change_who_speaks_on_moderated("+", attempt.getParams()[y]);
			y++;
		}
		else if (tmp[i] == 'k')
		{
			if (attempt.getParams().size() < (y + 1))
				return (-6); 
			ch.change_password("+", attempt.getParams()[y]);
		}
		else
			return (-7);
	}
	return (0);
}

int	modeCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	if (attempt.getParams().size() < 2)
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Channel* tmp = serv.get_channelPtr(attempt.getParams()[0]);
	if (tmp == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
	}
	if (!tmp->is_op(attempt.getSender()->getNickname()))
	{
		sendResponse(*(attempt.getSender()), Error::chanoprivsneeded(*(attempt.getSender()), attempt.getParams()[0]));
		return (-4);
	}
	int res;
	if (attempt.getParams()[1][0] == '-')
		res = minus(attempt, *tmp);
	else if (attempt.getParams()[1][0] == '+')
		res = plus(attempt, *tmp);
	else
		res = -7;
	switch (res)
	{
		case -5:
		{
			sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), attempt.getParams()[0])); //check
			break ;
		}
		case -6:
		{
			sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
			break;
		}
		case -7:
		{
			sendResponse(*(attempt.getSender()), Error::unknownmode(*(attempt.getSender()), attempt.getParams()[1])); //check
			break ;
		}
	}
	std::string msg = ":" + serv.get_name() + " 324 " +  attempt.getSender()->getNickname() + " " + tmp->get_name() + " +" + tmp->channel_modes() + "\r\n";
	tmp->broadcast(msg, 0);
	return (0);
}

int	listCommand(Server &serv, Message &attempt)
{
	sendResponse(*(attempt.getSender()), Reply::liststart(*(attempt.getSender())));
	if (attempt.getParams().size() == 0)
	{
		serv.list_allchannels(*attempt.getSender());
		sendResponse(*(attempt.getSender()), Reply::listend(*(attempt.getSender())));
		return (0);
	}
	std::vector<std::string> channels = split(attempt.getParams()[0], ",");
	std::size_t i = 0;
	while (i < channels.size())
	{
		Channel* tmp = serv.get_channelPtr(channels[i]);
		if (tmp != NULL)
		{
			if (tmp->get_is_private() || tmp->get_is_secret())
			{
				std::string msg2 = ":" + serv.get_name() + " 322 " + attempt.getSender()->getNickname() + " ";
				if (tmp->is_member(attempt.getSender()->getNickname()))
				{
					msg2 += tmp->get_name() + " :" + tmp->get_topic() + "\r\n";
					send(attempt.getSender()->getFd(), msg2.c_str(), msg2.length(), 0);
				}
				else
				{
					if (tmp->get_is_private() && !tmp->get_is_secret())
					{
						msg2 += tmp->get_name() + "\r\n";
						send(attempt.getSender()->getFd(), msg2.c_str(), msg2.length(), 0);
					}
				}
				msg2.clear();
			}
			else
			{
				std::string msg2 = ":" + serv.get_name() + " 322 " + attempt.getSender()->getNickname() + " ";
				msg2 += tmp->get_name() + " :" + tmp->get_topic() + "\r\n";
				send(attempt.getSender()->getFd(), msg2.c_str(), msg2.length(), 0);
			}
		}
		i++;
	}
	sendResponse(*(attempt.getSender()), Reply::listend(*(attempt.getSender())));
	return (0);
}

int	removeUserFromChannels(Server &serv, std::string nickname)
{
	std::map<std::string, Channel>::iterator	iter;

	iter = serv.getChannels().begin();
	while (iter != serv.getChannels().end())
	{
		iter->second.disconnect(nickname);
		++iter;
	}
	return (0);
}
