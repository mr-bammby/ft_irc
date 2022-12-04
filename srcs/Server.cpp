#include "Server.hpp"

#define PORT 6667


Server::Server(int port, std::string pass): server_port(port), used_clients(0), password(pass)
{}

Server::~Server()
{
	shutdown(this->pollfds[0].fd, SHUT_RDWR);
}

void Server::initExecutor()
{
	exeCommands.insert(std::pair<std::string, fun>("PASS", &passCommand));
	exeCommands.insert(std::pair<std::string, fun>("USER", &userCommand));
	exeCommands.insert(std::pair<std::string, fun>("NICK", &nickCommand));
	exeCommands.insert(std::pair<std::string, fun>("PRIVMSG", &privmsgCommand));
	exeCommands.insert(std::pair<std::string, fun>("NOTICE", &noticeCommand));
	exeCommands.insert(std::pair<std::string, fun>("KILL", &killCommand));
	exeCommands.insert(std::pair<std::string, fun>("QUIT", &quitCommand));
	exeCommands.insert(std::pair<std::string, fun>("PING", NULL));
	exeCommands.insert(std::pair<std::string, fun>("JOIN", &joinCommand));
	exeCommands.insert(std::pair<std::string, fun>("PART", &partCommand));
}

void Server::executor()
{
	Message *current;

	while (messages.size() > 0)
	{
		current = getNextMessage();
		std::cout << RED;
		std::cout << "Executing: " << current->getCommand()  << " TXT: "<<current->getText()<< std::endl;
		std::map<std::string, fun>::iterator it = exeCommands.find(current->getCommand());
		if (it == exeCommands.end())
		{
			std::cout<<"Command not found"<<std::endl;
			std::string msg = ":<servername> 421 " + current->getSender()->getNickname() + " " + current->getCommand() + " :Unknown command\r\n";
			send(current->getSender()->getFd(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			if (it->second != NULL)
				it->second(*this, *current);
		}
		std::cout << BLANK;
		removeLastMessage();
	}
}



int Server::init()
{
	int server_fd;
	struct sockaddr_in address;
	int opt = 1;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
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
	initExecutor();
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
		// changed iterator to start from second position, because first is server and it goes into if condition(gets deleted)
		for (std::vector<pollfd>::iterator pfdit = ++pollfds.begin(); pfdit != pollfds.end(); ++pfdit)
		{
			if (pfdit->revents & POLLIN)
			{
				char buf[1024];
				int buffsize = read(pfdit->fd, buf, 1024 - 1);
				if (buffsize == -1 || buffsize == 0)
				{
					std::cout<<"User disconnected"<<std::endl;
					// deleting disconected client from map of clients and deleting his pollfd from vector. 
					// put command break in the end because after erasing iterators could be invalid
					Client *tmp = get_clientPtr(pfdit->fd);
					this->clients_fdMap.erase(pfdit->fd);
					this->pollfds.erase(pfdit);
					this->clients_nameMap.erase(tmp->getNickname());
					this->used_clients--;
					shutdown(pfdit->fd, SHUT_RDWR);
					break ;
				}
				else{
					buf[buffsize] = '\0';
					// changed [] operator for function at().
					//probably leaking
					std::vector<Message> current = getMessages(buf, &(this->clients_fdMap.at(pfdit->fd)));
					messages.insert(messages.begin(), current.rbegin(), current.rend());
					current.clear();
					// this->clients.at(pfdit->fd).parse(buf);
					printf("Client: %s\n", buf);
					std::cout<<"Connected and reg. clients are: "<<std::endl;
					for (std::map<std::string, Client*>::iterator it = clients_nameMap.begin(); it != clients_nameMap.end(); it++)
					{
						std::cout<<"Nick: "<<it->second->getNickname()<<std::endl;
						std::cout<<"FD: "<<it->second->getFd()<<std::endl;
					}
					std::cout<<"End of clients-------------"<<std::endl;
				}
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
	// ToDo: check the password from the new User is correct
	pollfds.push_back(pollfd());
	pollfds.back().fd = client_socket;
	pollfds.back().events = POLLIN | POLLPRI;
	// creating client class and putting it in a map of clients, where the key is clients fd
	// std::pair<int, Client> t(client_socket, Client(used_clients, client_socket));
	clients_fdMap.insert(std::pair<int, Client>(client_socket, Client(used_clients, client_socket)));
	used_clients++;
	std::cout<<"-----------------------------Clients fd in create a client: "<<client_socket<<std::endl;
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
	std::cout<<"\n"<<"Set nick, nick: "<<nickName<<std::endl;
	std::cout<<"Set nick, FD: "<<client_ptr->getFd()<<"\n"<<std::endl;
	std::pair<std::map<std::string, Client*>::iterator, bool> temp;

	if (nickName.empty())
	{
		return (-1); // nickname is empty
	}
	if (client_ptr == NULL)
	{
		return (-2); // client_ptr is not viable pointer
	}
	if (checkNickGrammar(nickName) == -1)
	{
		return (-3); //sending ERR_ERRONEUSNICKNAME
	}
	std::map<std::string, Client*>::iterator itr = clients_nameMap.find(nickName);
	if (itr == clients_nameMap.end())
	{
		if (client_ptr->setNickname(nickName) == -8)
			return (-8);
		temp = clients_nameMap.insert(std::pair<std::string, Client*>(nickName, client_ptr));
		if (!temp.second)
			return (-4); // nick name alredy exist
		else
			return (-5);
	}
	else
	{
		std::map<std::string, Client*>::iterator itr2 = clients_nameMap.find(nickName);
		if (itr2 != clients_nameMap.end())
		{
			return (-6); // nick anme alredy exist ERR_NICKNAMEINUSE
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
	std::vector<pollfd>::iterator it = pollfds.begin();
	while (it->fd != user->getFd())
		it++;
	for (std::map<std::string, Channel>::iterator it1 = channels.begin(); it1 != channels.end(); it1++)
	{
		if (it1->second.is_member(user->getNickname()))
		{
			std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost PART " + it1->second.get_name() + "\r\n";
			it1->second.broadcast(msg, 0);
		}
		it1->second.disconnect(user->getNickname());
	}
	shutdown(it->fd, SHUT_RDWR);
	this->pollfds.erase(it);
	this->clients_nameMap.erase(user->getNickname());
	this->clients_fdMap.erase(user->getFd());
	used_clients--;
}
