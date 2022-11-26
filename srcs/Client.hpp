#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

#include <Message.hpp>

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
		Client(int id, int client_fd);
		Client();
		Client(const Client &c);
		~Client();

		Client &operator=(const Client &c);

		int 					parse(std::string command);
		const std::string&		getNickname();
		// DEBUG
		int			getFd()
		{
			return (client_fd);
		}
		int						setNickname(std::string name); //check for name uniqness before call of this function
		const Client::State&	getState();
		const std::string&		getUsername();
		int						setUsername(std::string name);
		const std::string&		getRealname();
		int						setRealname(std::string name);

	private:
		int				id;
		int				client_fd;
		std::string		nickname;
		Client::State	state;
		std::string		username;
		std::string		realname;

};

#endif // CLIENT_HPP
