#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "Client.hpp"

class Channel
{
	public:
		Channel(Client &c);
		Channel(const Channel &c);
		~Channel();

		Channel &operator=(const Channel &c);

		int	broadcast(std::string message);
		int	connect(int fd, Client &c);
		int	disconnect(int fd);
		int	cmd_kick(std::string);
		// int cmd_mode();
		int	cmd_invite(std::string nickname);
		int	cmd_topic(std::string topic);

		bool	is_op(Client &c);

	private:
		std::string					name;
		std::string					chanop;
		std::string					topic;
		std::map<int, Client*>		clients;
		bool						invite_only;
		std::vector<std::string>	invited_users;
};

#endif // CHANNEL_HPP
