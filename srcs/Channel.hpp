#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
	public:
		Channel(std::string op);
		Channel(const Channel &c);
		~Channel();

		Channel &operator=(const Channel &c);

		int	broadcast(std::string message);
		// int cmd_kick();
		// int cmd_mode();
		// int cmd_invite();
		// int cmd_topic();

	private:
		std::string name;
		std::string chanop;
		std::string topic;
		std::vector<Client> clients;
};

#endif // CHANNEL_HPP