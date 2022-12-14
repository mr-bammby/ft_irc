#ifndef BOT_HPP
# define BOT_HPP

#include "../srcs/Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class	Bot
{
	private:
		int awake;
		int bot_fd;
		struct sockaddr_in servAddr;
	
		std::string nick;
		std::string pass;
		std::string rawMessage;
		std::vector<std::string> messages;
		void	send_msg(std::string msg);
		void 	recieve_msg();
		void	join_server();
	public:
		Bot(int port, std::string pass);
		~Bot(void);

		void run();
};

#endif
