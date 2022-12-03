#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include "Client.hpp"

class Channel
{
	public:
		Channel(std::string name, const Client &c);
		Channel(const Channel &c);
		~Channel();

		Channel &operator=(const Channel &c);

		int	broadcast(std::string message);
		int	connect(const Client &c);
		int	disconnect(const Client &c);
		int	disconnect(std::string nickname);
		int	cmd_kick(std::string nickname);
		// int cmd_mode();
		int	cmd_invite(std::string nickname);
		int	cmd_topic(std::string topic);

		std::string	get_topic();

		bool	is_op(const Client &c);
		bool	is_op(std::String nickname);
		bool	is_member(const Client &c);
		bool	is_member(std::string nickname);
		bool	can_invite(const Client &c);
		bool	can_invite(std::string nickname);

	private:
		std::string						name;
		std::string						chanop;
		std::string						topic;
		std::map<std::string, Client*>	clients;
		bool							invite_only;
		std::vector<std::string>		invited_users;
};

#endif // CHANNEL_HPP
