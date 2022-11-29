#include "Comms.hpp"
#include <sstream>

int	executeCommands(Server &serv)
{
	Message *current;
	int	(*Table[])(Server &s, Message &a) =
	{
		&passCommand, &nickCommand, &userCommand
	};
	// TODO: get the actual's message type
	// const char *ComList[] =
	// {
	// 	"PASS", "NICK", "USER", "JOIN",
	// 	"PRIVMSG", "NOTICE", "KICK", "MODE",
	// 	"INVITE", "TOPIC", "KILL", "RESTART", NULL};
	while (serv.getBacklogLength() > 0)
	{
		current = serv.getNextMessage();
		std::cout<<"Type: "<<current->type<<std::endl;
		std::cout<<"Category: "<<current->category<<std::endl;
		std::cout << RED;
		std::cout << "Executing: " << current->command << " with: " << current->params[0] << std::endl;
		Table[current->type](serv, *current);
		std::cout << BLANK;
		serv.removeLastMessage();
	}
	return (0);
}

// Message createMessage(std::string str, Client *cl)
// {
// 	Message msg;
// 	msg.content = str;
// 	msg.sender = cl;
// 	return (msg);
// }

int	passCommand(Server &serv, Message &attempt)
{
	std::cout<<"provided pass: "<<attempt.params[0]<<std::endl;
	std::cout<<"Status: "<<attempt.sender->getState()<<std::endl;
	if (attempt.sender->getState() != 0)
	{
		std::cout << "Error for double password here" << std::endl;
		return (1);
	}
	if (serv.check_password(attempt.params[0]) == true)
	{
		std::cout << "Success!" << std::endl;
		attempt.sender->upgradeState();
	}
	else
		std::cout << "Failed!" << std::endl;
	std::cout << BLANK;
	return (0);
}


int	nickCommand(Server &serv, Message &attempt)
{
	int res = serv.set_nickName(attempt.sender, attempt.params[0]);
	std::cout<<"res: "<<res<<std::endl;
	switch(res)
	{
		case -1:
			break;// send ERR_NONICKNAMEGIVEN
			// break;
		case -2:
			//client_ptr is not viable pointer
			break;
		case -3:
			//sending ERR_ERRONEUSNICKNAME
			break;
		case -4:
			//sending ERR_NICKCOLLISION
			break;
		case -5:
			send(attempt.sender->getFd(), "001 * :- Welcome to irc server, bobo\r\n:irc 375 bobo :- irc Message of the Day -\r\n:irc 372 bobo Welcome to irc Server!\r\n:irc 376 bobo :End of /MOTD command.\r\n", 158, 0);
			//introducing new nick and //sending RPL_WELCOME message to client(Register connection)
			break;
		case -6:
			//sending ERR_NICKNAMEINUSE
			break;
		case -7:
			//"old nicname" changed his nickname to "new nickname"
			break;
		case -8:
			//sending ERR_NOTREGISTERED
			break;
	}

	return (0);
}

int	userCommand(Server &serv, Message &attempt)
{
	(void)serv;
	int res = attempt.sender->setUsername(attempt.params[0]);
	switch (res)
	{
		case -1:
			//sending ERR_NEEDMOREPARAMS 
			break;
		case -2:
			//sending ERR_NOTREGISTERED 
			break;
		case -3:
			//sending ERR_ALREADYREGISTERED
			break;
		case -4:
			attempt.sender->setRealname(attempt.params.back());
			break;
	}

	return (0);
}

int	joinCommand(Server &serv, Message &attempt)
{
	if (attempt.params.size() == 0)
		return (-1); // sending ERR_NEEDMOREPARAMS 

	//extracting channels and passwords
	std::vector<std::string> channels = split(attempt.params[0], ",");
	std::vector<std::string> passwords = split(attempt.params[1], ",");

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
			if (channels[i][0] != '#' || channels[i][0] != '&')
				return (-2); // sending ERR_NOSUCHCHANNEL
			// creating channel with channells[i] as a name and making client as a operator
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
		}
		i++;
	}

	return (0);
}

int	privmsgCommand(Server &serv, Message &attempt)
{
	if (attempt.params.size() == 0)
		return (-1); // sending ERR_NORECIPIENT
	if (attempt.params.size() == 1)
		return (-2); // sending ERR_NOTEXTTOSEND
	
	std::vector<std::string> recipients = split(attempt.params[0], ",");
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
				return (-3); // sending ERR_NOSUCHNICK
			else
			{
				//sending to channel
				// if client in channel is away, sending RPL_AWAY message
			}
		}
		else
		{
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
	if (attempt.params.size() == 0)
		return (-1); 
	if (attempt.params.size() == 1)
		return (-2);
	
	std::vector<std::string> recipients = split(attempt.params[0], ",");
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
				return (-3);
			else
			{
				//sending to channel
			}
		}
		else
		{
			// sending to client
		}
		
		it++;
	}

	return (0);
}
