#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include "Client.hpp"

/** @class Channel
 * @brief This class handless all interactions connected to irc channel operation.
 * 
 * @details It includes handling of members, topic, operator. 
 * 			It also have functionality of inviting, joining, and removing of members.
 * 			Meant to be used with proprietary class Client
 */
class Channel
{
	public:
		Channel(std::string name, Client &c);
		~Channel();
		Channel &operator=(const Channel &c);

		//actions
		int	broadcast(std::string message, int sender);
		int	connect(Client &c);
		int	disconnect(Client &c);
		int	disconnect(std::string nickname);
		int	cmd_kick(std::string nickname);
		int	cmd_invite(std::string nickname);
		int	cmd_topic(std::string top);
		int	cmd_names(Client& sender);
		int cmd_who(Client& sender);
		int	add_operator(std::string nick);
		int	change_operator(std::string sign, std::string nick);
		int	client_count();
		int	list_coms(Client& sender);

		//setters
		int		change_is_private(std::string sign);
		int		change_is_secret(std::string sign);
		int		change_optopic(std::string sign);
		int		change_nomsg(std::string sign);
		int		change_moderated(std::string sign);
		int		change_userlimits(std::string sign, std::size_t limit);
		int		change_password(std::string sign, std::string key);
		int		change_invite(std::string sign);
		int		change_who_speaks_on_moderated(std::string sign, std::string user);

		//getters 
		std::string					get_topic();
		std::string					get_name() const;
		Client*						get_op() const;
		bool						is_invited(std::string nick);
		bool						is_op(std::string nickname);
		bool						is_member(Client &c);
		bool						is_member(std::string nickname);
		bool						can_invite(Client &c);
		int							can_invite(std::string nickname);
		bool						get_invite_only() const;
		bool						get_is_private() const;
		bool						get_is_secret() const;
		bool						get_op_topic() const;
		bool						get_no_msg() const;
		bool						get_moderated() const;
		int							get_user_limit() const;
		std::string					get_password() const;
		std::vector<std::string>	get_who_speaks_on_moderated() const;
		std::vector<std::string>	get_invited_users() const;
		std::vector<std::string>	get_operators() const;
		bool						can_speak_onchannel(std::string nick);
		bool						limit_full();
		std::string					channel_modes();

	private:
		
		std::string						name;
		std::string						owner;
		std::vector<std::string>		operators;
		std::string						topic;
		std::map<std::string, Client*>	clients;
		bool							invite_only;
		std::vector<std::string>		invited_users;
		bool							is_private;
		bool							is_secret;
		bool							op_topic;
		bool							no_msg;
		bool							moderated;
		std::size_t						user_limit;
		std::string						password;
		std::vector<std::string>		who_speaks_on_moderated;

		Channel(const Channel &c); //canonical form				

};

#endif // CHANNEL_HPP
