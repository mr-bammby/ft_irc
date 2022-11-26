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
		int set_nickName(Client* client_ptr, std::string nickName);
		Client* get_clientPtr(int fd);
		Client* get_clientPtr(std::string nickName);
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
		std::map<int, Client> clients_fdMap;
		std::map<std::string, Client*> clients_nameMap;
		std::vector<Channel> channels;	
};


#endif // SERVER_HPP
