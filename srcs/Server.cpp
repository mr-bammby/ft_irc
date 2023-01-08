#include "Server.hpp"

#define PORT 6667


Server::Server(int port, std::string pass): on(true), server_port(port), used_clients(0), password(pass), name("IRC")
{
	setupTable();
}

Server::~Server()
{
	cleanTable();
	shutdown(this->pollfds[0].fd, SHUT_RDWR);
	for (std::vector<pollfd>::iterator pfdit = pollfds.begin(); pfdit != pollfds.end(); ++pfdit)
    {
        close((*pfdit).fd);
    }

}

std::string		Server::get_name()
{
	return (name);
}

int				Server::get_serverfd()
{
	return (server_fd);
}

void Server::executor()
{

	Message *current;

	while (messages.size() > 0)
	{
		current = getNextMessage();
		std::cout << RED;

		std::cout<<"COMMANDS IN EXECUTOR, CATEGORY: "<<current->getComCategory()<<" TYPE: "<<current->getType()<<std::endl;
		if (current->getComCategory() != IGNORE && current->getType() != UNKNOWN)
			if (this->table[current->getComCategory()][current->getType() % 10] != NULL)
				this->table[current->getComCategory()][current->getType() % 10](*this, *current);
		if (current->getType() == UNKNOWN)
			sendResponse(*(current->getSender()), Error::unknowncommand(*(current->getSender()), current->getCommand()));
		std::cout << BLANK;
		removeLastMessage();
	}
}

int Server::init()
{
	struct sockaddr_in address;
	int opt = 1;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK)) {
		perror("Could not set non-blocking socket");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET,
					SO_REUSEADDR, &opt,
					sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	// changed to port which passed in the begining
	address.sin_port = htons(server_port);

	// Forcefully attaching socket to the port 6667
	if (bind(server_fd, (struct sockaddr*)&address,
			 sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	pollfds.push_back(pollfd());
	pollfds[0].fd = server_fd;
	pollfds[0].events = POLLIN | POLLPRI;
	return 0;
}

int Server::start_loop()
{
	int pollResult = poll(&pollfds[0], this->used_clients + 1, 5000);
	if (pollResult > 0)
	{
		if (pollfds[0].revents & POLLIN)
		{
			create_client();
		}
		for (std::vector<pollfd>::iterator pfdit = ++pollfds.begin(); pfdit != pollfds.end(); ++pfdit)
		{
			if (pfdit->revents & POLLIN)
			{
				char buf[1024];
				int buffsize = read(pfdit->fd, buf, 1024 - 1);
				if (buffsize == -1 || buffsize == 0)
				{
					std::cout<<"User disconnected"<<std::endl;
					Client *tmp = get_clientPtr(pfdit->fd);
					deleteUser(tmp);
				}
				else{
					buf[buffsize] = '\0';
					std::vector<Message> current = getMessages(buf, &(this->clients_fdMap.at(pfdit->fd)), incomplete);
					//std::cout <<"666666666" << std::endl;
					messages.insert(messages.begin(), current.rbegin(), current.rend());
					//std::cout <<"7777777" << std::endl;
					current.clear();

					//printf("Client: %s\n", buf);
				}
				break ;
			}
		}
	}
	return (0);

}

int Server::create_client()
{
	struct sockaddr_in client_address;
	socklen_t client_address_size = sizeof(client_address);
	int client_socket = accept(pollfds[0].fd, (struct sockaddr *)&client_address, &client_address_size);
	if (client_socket == -1)
	{
		std::cerr << "Error occured while accept incoming Connection!" << std::endl;
		return (-1);
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK)) {
		perror("Could not set non-blocking socket");
		exit(EXIT_FAILURE);
	}
	pollfds.push_back(pollfd());
	pollfds.back().fd = client_socket;
	pollfds.back().events = POLLIN | POLLPRI;
	// creating client class and putting it in a map of clients, where the key is clients fd
	// std::pair<int, Client> t(client_socket, Client(used_clients, client_socket));
	clients_fdMap.insert(std::pair<int, Client>(client_socket, Client(used_clients, client_socket)));
	used_clients++;
	return (0);
}

void	Server::cmd_namesAllchannels(Client& c)
{
	for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
		it->second.cmd_names(c);
}

int		Server::list_allchannels(Client& c)
{
	std::map<std::string, Channel>::iterator it = channels.begin();
	while (it != channels.end())
	{
		if (it->second.get_is_private() || it->second.get_is_secret())
		{
			std::string msg2 = ":" + get_name() + " 322 " + c.getNickname() + " ";
			if (it->second.is_member(c.getNickname()))
			{
				std::cout<<"Member"<<std::endl;
				std::string		topic = it->second.get_topic();
				if (topic.size() == 0)
					topic = "No topic";
				msg2 += it->second.get_name() + " : " + topic + "\r\n";
				send(c.getFd(), msg2.c_str(), msg2.length(), 0);
			}
			else
			{
				if (it->second.get_is_private() && !it->second.get_is_secret())
				{
					msg2 += it->second.get_name() + "\r\n";
					send(c.getFd(), msg2.c_str(), msg2.length(), 0);
				}
			}
			msg2.clear();
		}
		else
		{
			std::string msg2 = ":" + get_name() + " 322 " + c.getNickname() + " ";
			std::string		topic = it->second.get_topic();
			if (topic.size() == 0)
				topic = "No topic";
			msg2 += it->second.get_name() + " : " + topic + "\r\n";
			send(c.getFd(), msg2.c_str(), msg2.length(), 0);
		}
		it++;
	}
	return (0);
}

Channel* Server::create_channel(std::string name, Client& c)
{
	return (&this->channels.insert(std::pair<std::string, Channel>(name, Channel(name, c))).first->second);
	// return (0);
}

bool	Server::check_password(std::string pass)
{
	std::cout << "In check pass: " << pass << " and " << password << std::endl;
	if (password == pass)
		return (true);
	return (false);
}

Message *Server::getNextMessage()
{
	return &(*(--(this->messages.end())));
}

int		Server::getBacklogLength()
{
	return (this->messages.size());
}

void	Server::removeLastMessage()
{
	this->messages.pop_back();
}

int Server::checkNickGrammar(std::string nick)
{
	std::string allowedChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-[]\\`^{}";
	int i = 0;
	while (nick[i])
	{
		if (allowedChar.find(nick[i]) == std::string::npos)
			return (-1);
		i++;
	}

	return (0);
}

int Server::set_nickName(Client* client_ptr, std::string nickName)
{
	std::pair<std::map<std::string, Client*>::iterator, bool> temp;

	if (nickName.empty())
	{
		return (-1);
	}
	if (client_ptr == NULL)
	{
		return (-2); // client_ptr is not viable pointer
	}
	if (checkNickGrammar(nickName) == -1 || nickName.size() > 9)
	{
		return (-3);
	}
	std::map<std::string, Client*>::iterator itr = clients_nameMap.find(nickName);
	if (itr == clients_nameMap.end())
	{
		std::cout << "intro here" << std::endl;
		int i = client_ptr->setNickname(nickName);
		if (i == -8)
			return (-8);
		if (i == -7)
			return (-7);
		temp = clients_nameMap.insert(std::pair<std::string, Client*>(nickName, client_ptr));
		if (!temp.second)
			return (-4); //nickname in use
		else
			return (-5);
	}
	else
	{
		std::cout << "or there" << std::endl;
		std::map<std::string, Client*>::iterator itr2 = clients_nameMap.find(nickName);
		if (itr2 != clients_nameMap.end())
		{
			return (-6); // nickname in use 2, can we use -4 here?
		}
		else
			return (-7);// probably have to send message "old nicname" changed his nickname to "new nickname"
	}
	
	return (0);
}

Client* Server::get_clientPtr(std::string nickName)
{
	std::map<std::string, Client*>::iterator itr = clients_nameMap.find(nickName);
	if (itr == clients_nameMap.end())
	{
		return (NULL);
	}
	return(itr->second);
}

Client* Server::get_clientPtr(int fd)
{
	std::map<int, Client>::iterator itr = clients_fdMap.find(fd);
	if (itr == clients_fdMap.end())
	{
		return (NULL);
	}
	return(&(itr->second));
}

Channel* Server::get_channelPtr(std::string chan)
{
	std::map<std::string, Channel>::iterator itr = channels.find(chan);
	if (itr == channels.end())
	{
		return (NULL);
	}
	return(&(itr->second));
}

void	 Server::deleteUser(Client *user)
{
	std::vector<std::string> to_delete;
	std::vector<pollfd>::iterator it = pollfds.begin();
	while (it->fd != user->getFd())
		it++;
	for (std::map<std::string, Channel>::iterator it1 = channels.begin(); it1 != channels.end(); it1++)
	{
		if (it1->second.is_member(user->getNickname()))
		{
			std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost QUIT " + it1->second.get_name() + "\r\n";
			it1->second.broadcast(msg, 0);
			it1->second.disconnect(user->getNickname());
			if (it1->second.client_count() == 0)
				to_delete.push_back(it1->second.get_name());
				// channels.erase(it1->second.get_name());
		}
	}
	for (std::vector<std::string>::iterator i = to_delete.begin(); i != to_delete.end(); i++)
	{
		channels.erase(*i);
	}
	shutdown(it->fd, SHUT_RDWR);
	close(it->fd);
	this->pollfds.erase(it);
	this->clients_nameMap.erase(user->getNickname());
	this->clients_fdMap.erase(user->getFd());
	this->incomplete.erase(user);
	used_clients--;
}

int		Server::deleteChannel(std::string name)
{
	channels.erase(name);
	return (0);
}

std::map<std::string, Channel>	&Server::getChannels()
{
	return (this->channels);
}

typedef int (*fun)(Server&, Message&);

void	Server::setupTable()
{
	fun		*res[6];

	res[0] = new fun[3];
	res[0][0] = &passCommand;
	res[0][1] = &nickCommand;
	res[0][2] = &userCommand;
	res[1] = new fun[2];
	res[1][0] = &privmsgCommand;
	res[1][1] = &noticeCommand;
	res[2] = new fun[9];
	res[2][0] = &kickCommand;
	res[2][1] = &modeCommand;
	res[2][2] = &inviteCommand;
	res[2][3] = &topicCommand;
	res[2][4] = &partCommand;
	res[2][5] = &whoCommand;
	res[2][6] = &namesCommand;
	res[2][7] = &listCommand;
	res[2][8] = &joinCommand;
	res[3] = new fun[2];
	res[4] = new fun[2];
	res[5] = new fun[5];
	res[5][0] = &killCommand;
	res[5][1] = NULL;
	res[5][2] = &operCommand;
	res[5][3] = &quitCommand;
	res[5][4] = &squitCommand;
	for (int i = 0; i < 6; i++)
	{
		this->table.push_back(res[i]);
	}
}

void	Server::cleanTable()
{
	for (int i = 0; i < 6; i++)
	{
		delete[] this->table[i];
	}
	this->table.clear();
}

std::vector<pollfd>&		Server::get_pollfds()
{
	return(pollfds);
}
