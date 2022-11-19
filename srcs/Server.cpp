#include "Server.hpp"

#define PORT 6667


Server::Server(int port, std::string pass): server_port(port), password(pass)
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
					SO_REUSEADDR | SO_REUSEPORT, &opt,
					sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

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
			for (std::vector<pollfd>::iterator pfdit = pollfds.begin(); pfdit != pollfds.end(); ++pfdit)
			{
				if (pfdit->revents & POLLIN)
				{
					char buf[1024];
					int buffsize = read(pfdit->fd, buf, 1024 - 1);
					if (buffsize == -1 || buffsize == 0)
					{
						pfdit->fd = 0;
						pfdit->events = 0;
						pfdit->revents = 0;
						this->used_clients--;
					}
					else{
						buf[buffsize] = '\0';
						this->clients[i].parse(buf);
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
	used_clients++;

	return (0);
}

int Server::create_channel()
{
	//if (check_channel_name());
	//{
	//	return (-1);
	//}

}
