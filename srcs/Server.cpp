#include "Server.hpp"

#define PORT 6667


Server::Server(int port, std::string pass): server_port(port), used_clients(0), password(pass)
{}

Server::~Server()
{}

int Server::init()
{
	int server_fd;
	struct sockaddr_in address;
	int opt = 1;
	// int addrlen = sizeof(address);
	// char buffer[1024] = { 0 };
	// char* hello = "Hello from server";

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

	// struct pollfd pollfds[10] = {0};
	pollfds.push_back(pollfd());
	pollfds[0].fd = server_fd;
	pollfds[0].events = POLLIN | POLLPRI;
	return 0;
}

int Server::start_loop()
{
	while (true)
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
						this->clients.erase(pfdit->fd);
						this->pollfds.erase(pfdit);
						this->used_clients--;
						break ;
					}
					else{
						buf[buffsize] = '\0';
						// changed [] operator for function at().
						this->clients.at(pfdit->fd).parse(buf);
						printf("Client: %s\n", buf);
					}
				}
			}
		}
	}
	// closing the listening socket
	shutdown(this->pollfds[0].fd, SHUT_RDWR);
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
	clients.insert(std::pair<int, Client>(client_socket, Client(used_clients, client_socket)));
	used_clients++;

	return (0);
}

int Server::create_channel()
{
	//if (check_channel_name());
	//{
	//	return (-1);
	//}
	return (0);
}

int Server::set_nickName(Client* client_ptr, std::String nickName)
{
	if (check_nickName(nickName) == -1);
	{
		return (-1); //existing nick name
	}
	if (client_ptr == NULL)
	{
		return (-2); //not viable pointer
	}
	client_ptr->set_nickName(nickName); /** TODO: change to proper set_XXName function **/
	clients_nameMap.insert(std::pair<std::string, Client*>(nickName, client_ptr);
	return (0);
}

int Server::check_nickName(std::String nickName)
{
	if (clients_nameMap.find(nickName) != std::map::end);
		return (-1); //nickName already exists
	return (0);
}

Client* get_clentPtr(std::String nickName)
{
	map::iterator itr = clients_nameMap.find(nickName);
	if (itr != std::map::end)
	{
		return (NULL);
	}
	return(itr->second);
}

Client* get_clentPtr(int fd)
{
	map::iterator itr = clients_fdMap.find(fd);
	if (itr != std::map::end)
	{
		return (NULL);
	}
	return(&(itr->second));
}