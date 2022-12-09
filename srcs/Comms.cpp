#include "Comms.hpp"
#include <sstream>

// int	executeCommands(Server &serv)
// {
// 	Message *current;
// 	int	(*Table[])(Server &s, Message &a) =
// 	{
// 		&passCommand, &nickCommand, &userCommand, &joinCommand
// 	};
// 	// TODO: get the actual's message type
// 	// const char *ComList[] =
// 	// {
// 	// 	"PASS", "NICK", "USER", "JOIN",
// 	// 	"PRIVMSG", "NOTICE", "KICK", "MODE",
// 	// 	"INVITE", "TOPIC", "KILL", "RESTART", NULL};
// 	while (serv.getBacklogLength() > 0)
// 	{
// 		current = serv.getNextMessage();
// 		std::cout << RED;
// 		std::cout << "Executing: " << current->getCommand() << " with: " << current->getParams()[0] << std::endl;
// 		Table[current->getType()](serv, *current);
// 		std::cout << BLANK;
// 		serv.removeLastMessage();
// 	}
// 	return (0);
// }

// Message createMessage(std::string str, Client *cl)
// {
// 	Message msg;
// 	msg.content = str;
// 	msg.sender = cl;
// 	return (msg);
// }

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
	if (serv.check_password(attempt.getText()) == true)
	{
		std::cout << "Success!" << std::endl;

		attempt.getSender()->upgradeState();
	}
	else
		std::cout << "Failed!" << std::endl;
	std::cout << BLANK;
	return (0);
}

void introducing(Client *sender)
{
	sendResponse(*sender, Reply::welcome(*sender));
	sendResponse(*sender, Reply::motdstart(*sender));
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
			introducing(attempt.getSender());
			//introducing new nick and //sending RPL_WELCOME message to client(Register connection)
			break;
		case -6:
		{
			sendResponse(*(attempt.getSender()), Error::nicknameinuse(attempt.getParams()[0]));
			break ; //is this intentionally here twice??
		}
		case -7:
			//"old nicname" changed his nickname to "new nickname"
			break;
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
		return (-2); // This might be redundant
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
			attempt.getSender()->setRealname(attempt.getText());
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
			tmp->connect(*attempt.getSender());
			std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost JOIN" + " :" + tmp->get_name() + "\r\n";
			tmp->broadcast(msg, 0);
			tmp->cmd_names(*attempt.getSender());
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
		Client* tmp = serv.get_clientPtr(*it);
		if (tmp == NULL)
		{
			Channel* tmp2 = serv.get_channelPtr(*it);
			if (tmp2 == NULL)
			{
				sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), "PLACEHOLDER")); //what var?
				return (-4);
			}
			else
			{
				//sending to channel
				std::string	message;
				// message = ":boriss PRIVMSG bobo :aaaa\r\n";
				message = ":" + attempt.getSender()->getNickname() + " PRIVMSG " + tmp2->get_name() + " :" + attempt.getText() + "\r\n";
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
			message = ":" + attempt.getSender()->getNickname() + " PRIVMSG " + tmp->getNickname() + " :" + attempt.getText() + "\r\n";
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
				std::string	message;
				message = ":" + attempt.getSender()->getNickname() + " NOTICE " + tmp2->get_name() + " :" + attempt.getText() + "\r\n";
				std::cout<<"Sending message: "<<message<<std::endl;
				tmp2->broadcast(message, attempt.getSender()->getFd());
				// send(tmp->getFd(), message.c_str(), message.length(), 0);
				message.clear();
			}
		}
		else
		{
			std::string	message;
			message = ":" + attempt.getSender()->getNickname() + " NOTICE " + tmp->getNickname() + " :" + attempt.getText() + "\r\n";
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
		return (-3); //Unknown server 
 	}

	if (chn == NULL)
	{
		return (-4); //Unknown server
 	}
	/*
	int chn_resp = chn.can_invite(*chn, *cln);
	if (chn_resp == 0)
	{
		//send invite
	}
	else if (chn_resp == CHN_ERR_INVITE_NOT_IN_CHN)
	{
		//ERR_NOTONCHANNEL
	}
	else if (chn_resp == CHN_ERR_INVITE_NO_PRIV)
	{
		//ERR_CHANOPRIVSNEEDED
	}
	else if (chn_resp == CHN_ERR_INVITE_ALREADY_INVITED)
	{
		//ERR_USERONCHANNEL
	}
	else
	{
		//UNKNOWN_ERR
	}
	*/
	return (0);


}

int	killCommand(Server &serv, Message &attempt)
{
	if (attempt.getSender()->getState() != 3)
	{
		sendResponse(*(attempt.getSender()), Error::notregistered());
		return (-1);
	}
	// if (attempt.getSender() is not operator)
	// 	return (-1); //sending ERR_NOPRIVILEGES
	if (attempt.getParams().empty())
	{
		sendResponse(*(attempt.getSender()), Error::needmoreparams(attempt.getCommand()));
		return (-2);
	}
	Client* tmp = serv.get_clientPtr(attempt.getParams()[0]); //this looks like it will segfault if empty
	if (tmp == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchnick(*(attempt.getSender()), "PLACEHOLDER")); //where is this stored
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
			return (-3); //sending ERR_NOSUCHCHANNEL
		else if (!tmp->is_member(attempt.getSender()->getNickname()))
			return (-4); //sending ERR_NOTONCHANNEL
		std::string msg = ":" + attempt.getSender()->getNickname() + "!" + attempt.getSender()->getUsername() + "@localhost PART " + channels[i] + "\r\n";
		tmp->broadcast(msg, 0);
		tmp->disconnect(attempt.getSender()->getNickname());
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
	Channel* tmp = serv.get_channelPtr(attempt.getParams()[0]); //will most likely segfault if params empty
	if (tmp == NULL)
	{
		sendResponse(*(attempt.getSender()), Error::nosuchchannel(*(attempt.getSender()), attempt.getParams()[0]));
		return (-3);
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
		serv.cmd_namesAllchannels(attempt.getSender());
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
	if (attempt.getText().size() == 0)
	{
		if (tmp->get_topic().size() == 0)
		{
			std::string msg = ":<servername> 331 " + attempt.getSender()->getNickname() + " " + tmp->get_name() + " :No topic is set\r\n";
			send(attempt.getSender()->getFd(), msg.c_str(), msg.length(), 0);
			return (0);
		}
		std::string msg = ":<servername> 332 " + attempt.getSender()->getNickname() + " " + tmp->get_name() + " :" + tmp->get_topic() + "\r\n";
		send(attempt.getSender()->getFd(), msg.c_str(), msg.length(), 0);
	}
	else
	{
		if (!tmp->is_op(attempt.getSender()->getNickname()))
		{
			sendResponse(*(attempt.getSender()), Error::chanoprivsneeded(*(attempt.getSender()), attempt.getParams()[0]));
			return (-5);
		}
		tmp->cmd_topic(attempt.getText());
		std::string msg = ":<servername> 332 " + attempt.getSender()->getNickname() + " " + tmp->get_name() + " :" + tmp->get_topic() + "\r\n";
		send(attempt.getSender()->getFd(), msg.c_str(), msg.length(), 0);
		return (0);
	}
	return (0);
}

int	removeUserFromChannels(Server &serv, std::string nickname)
{
	std::vector<Channel>::iterator	iter;

	iter = serv.getChannels().begin();
	while (iter != serv.getChannels().end())
	{
		iter->disconnect(nickname);
		++iter;
	}
	return (0);
}