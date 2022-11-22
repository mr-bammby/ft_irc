#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

#define MAX_CLIENTS 128

class Server
{
	public:
		Server(int port, std::string password);
		~Server();
		int init();
		int start_loop();
		int create_channel();
		bool	check_password(std::string pass);
	private:
		Server();
		Server(const Server &s);
		Server	&operator=(const Server &s);
		int create_client();
		int server_port;
//		std::vector<int> user_ports;
		int used_clients;
		std::string password;
		// struct pollfd pollfds[128];
		std::vector<pollfd> pollfds;
		std::map<int, Client> clients;
		std::vector<Channel> channels;
};


#endif // SERVER_HPP
