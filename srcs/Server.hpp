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

#define MAX_CLIENTS 128
#define TIMEOUT 	180

/** @class Server
 * @brief This class handless server operation based.
 * 
 * @details The main class of irc server.
 */
class Server
{
	public:
		Server(int port, std::string password);
		~Server();
		int 								init();
		int 								start_loop();
		Channel*							create_channel(std::string name, Client& c);
		bool								check_password(std::string pass);
		Message 							*getNextMessage();
		int									getBacklogLength();
		void								removeLastMessage();
		int 								checkNickGrammar(std::string nick);
		int 								set_nickName(Client* client_ptr, std::string nickName);
		Client* 							get_clientPtr(int fd);
		Client* 							get_clientPtr(std::string nickName);
		Channel* 							get_channelPtr(std::string chan);
		void								pingClients();
		void								deleteUser(Client *user);
		void 								initExecutor();
		void 								executor();
		void								cmd_namesAllchannels(Client& c);
		std::string							get_name();
		int									get_serverfd();
		std::vector<pollfd>&				get_pollfds();
		int									list_allchannels(Client& c);
		int									deleteChannel(std::string name);
		std::map<std::string, Channel>		&getChannels();

		typedef int (*fun)(Server&, Message&);
		bool								on;

	private:
		Server();
		Server(const Server &s);
		Server	&operator=(const Server &s);
		int									create_client();
		void								setupTable();
		void								cleanTable();

		int 								server_port;
		int 								used_clients;
		std::string 						password;
		std::vector<pollfd> 				pollfds;
		std::map<int, Client> 				clients_fdMap;
		std::map<std::string, Client*>		clients_nameMap;
		std::map<std::string, Channel>		channels;
		std::vector<Message> 				messages;
		std::map<Client*, std::string>		incomplete;
		std::string							name;
		std::vector<fun*>					table;
		int									server_fd;
};


#endif // SERVER_HPP
