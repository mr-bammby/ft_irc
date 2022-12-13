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
#include <fcntl.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Comms.hpp"
#include "ServerResponse.hpp"


class Server
{
	public:
		Server(int port, std::string password);
		~Server();
		int 								init();
		int 								start_loop();
		void 								initExecutor();
		void 								executor();

		// getters
		Message 							*getNextMessage();
		int									getBacklogLength();
		Client* 							get_clientPtr(int fd);
		Client* 							get_clientPtr(std::string nickName);
		Channel* 							get_channelPtr(std::string chan);
		std::string							get_name();
		int									get_serverfd();
		std::map<std::string, Channel>		&getChannels();

		//settters and helpers

		Channel* 							create_channel(std::string name, Client& c);
		bool								check_password(std::string pass);
		void								removeLastMessage();
		int 								checkNickGrammar(std::string nick);
		int 								set_nickName(Client* client_ptr, std::string nickName);
		void								deleteUser(Client *user);
		void								cmd_namesAllchannels(Client& c);
		int									list_allchannels(Client& c);
		int									deleteChannel(std::string name);

		//variables
		typedef int (*fun)(Server&, Message&);
		bool					   			on;

	private:

		int 								create_client();

		int 								server_port;
		int 								used_clients;
		std::string 						password;
		std::vector<pollfd> 				pollfds;
		std::map<int, Client> 				clients_fdMap;
		std::map<std::string, Client*> 		clients_nameMap;
		std::map<std::string, Channel> 		channels;
		std::vector<Message> 				messages;
		std::map<Client*, std::string>		incomplete;
		std::string				   			name;
		int						   			server_fd;
		std::map<std::string, fun> 			exeCommands;
};


#endif // SERVER_HPP
