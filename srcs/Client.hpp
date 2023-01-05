#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

#include <Message.hpp>

/** @class Client
 * @brief This class handless all interactions connected to irc client operation.
 * 
 * @details It includes handling of names, states and fd of client.
 * 			Also handless password check an name setting procedure.
 * 			Meant to be used with proprietary class Message.
 */
class Client
{
	public:
		enum State			//states client can be in in relation to server
		{
			LOCKED,			//waiting for password check
			UNINITIALIZED,	//before setting nickname
			INITIALIZED,	//after setting nickname (waiting for second irc message)
			SET 			//ready ro communicate
		};


	public:
		Client(int _id, int client_fd);
		Client();
		~Client();

		Client &operator=(const Client &c);

		//action
		int 					parse(std::string command);

		//getters
		const std::string&		getNickname();
		const Client::State&	getState();
		const std::string&		getUsername();
		const std::string&		getRealname();
		int						setRealname(std::string name);
		void					upgradeState();
		int						getFd();
		bool					is_op();

		//setters
		int						setNickname(std::string name); //check for name uniqueness before call of this function
		bool					set_op(std::string pass);
		int						setUsername(std::string name);

	private:
		int				id;
		int				client_fd;
		std::string		nickname;
		Client::State	state;
		std::string		username;
		std::string		realname;
		bool			op;

};

#endif // CLIENT_HPP
