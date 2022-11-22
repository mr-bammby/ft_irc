#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <Commands.hpp>

struct Message
{
	enum ComCategory command_category;
	int comand_type;
	std::vector<std::string> params;
};


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
		int						setNickname(std::string name); //check for name uniqness before call of this function
		const Client::State&	getState();

	private:
		int				id;
		int				client_fd;
		std::string		nickname;
		Client::State	state;

		std::vector<std::string> split(const std::string& str, const std::string& delimiter);
		std::pair<enum ComCategory, int> detectMsgType(const std::string& token);
};

#endif // CLIENT_HPP
