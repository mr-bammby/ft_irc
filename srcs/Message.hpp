#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Commands.hpp"

class Client;

struct prefix {
	std::string severname;

	std::string nick;
	std::string user;
	std::string host;

	std::string buildShortPrefix() const;
	std::string buildLongPrefix() const;
};

class Message
{

public:

	Message(const std::string& rawMsg, Client* sender);
	Message(
		enum Commands cmdType,
		const std::vector<std::string>& params,
		Client*							sender);
	~Message();

	//getter
	const std::string&				getCommand() const;
	const std::vector<std::string>& getParams() const;
	Client*							getSender() const;
	std::string						getText() const;

	//setters
	void setCommand(enum Commands cmd);
	void setParams(const std::vector<std::string>& params);
	void setSender(Client* sender);

	std::string buildRawMsg() const;


private:
	struct prefix			 prefix;
	std::string		 		 client_prefix;
	enum ComCategory		 category;
	enum Commands			 type;
	std::string				 command;
	std::vector<std::string> params;
	Client*					 sender;

	std::string getCommandStr(enum Commands cmd_type);

	void find_prefx(std::string *raw, std::string *prefix); // finds and removes prefix
	void find_last_param(std::string *raw, std::string *last_param); // finds and removes last parameter

};

std::vector<Message> getMessages(const std::string& raw, Client* sender, \
std::map<Client*, std::string> &incomplete_map);
std::vector<std::string>	split(const std::string& str, const std::string& delimiter);


#endif // MESSAGE_HPP
