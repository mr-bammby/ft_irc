#ifndef BOT_HPP
# define BOT_HPP

#include "../srcs/Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/**
 * this is the dumbest thing ever written haha
 * currently segfaults if server quits
*/
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
		Bot(Bot const &src);
		~Bot(void);
		//Bot &operator=(Bot const &rhs);

		void run();
};

#endif
