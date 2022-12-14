#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include "Message.hpp"

class Client
{
	public:
		enum State
		{
			LOCKED,			//waiting for password check
			UNINITIALIZED,	//before setting nickane
			INITIALIZED,	//after setting nickname (waiting for second irc message)
			SET 			//ready ro communicate
		};


	public:
		Client(int cl_fd);
		Client();
		~Client();

		//getters
		const std::string&		getNickname();
		const Client::State&	getState();
		const std::string&		getUsername();
		const std::string&		getRealname();
		int						getFd();
		bool					is_op();

		//setters
		int						setNickname(std::string name);
		int						setUsername(std::string name);
		int						setRealname(std::string name);
		void					upgradeState();
		bool					set_op(std::string pass);

	private:
		int				client_fd;
		std::string		nickname;
		Client::State	state;
		std::string		username;
		std::string		realname;
		bool			op;

};

#endif // CLIENT_HPP
