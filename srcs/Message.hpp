#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <CommandList.hpp>
#include <utility.hpp>
#include <Client.hpp>

class Client;
// TODO: add test for this file

// prefix identifies the sender. Server fills this before realyed to a client
struct prefix {
	std::string nick;
	std::string user;
	std::string host;

	std::string buildRawPrefix() const;
};

class Message
{
public:
	Message();
	Message(const std::string& rawMsg, Client *cl = NULL);
	// Message(const Message& other);
	~Message();

	// Message& operator=(const Message& other);

	std::string buildRawMsg() const;

	friend std::ostream& operator<<(std::ostream& os, const Message& msg);
//temporary
public:
	struct prefix			 prefix; // empty in msg from client to server
	enum ComCategory		 category;
	int						 type;
	Client					 *client;
	std::string				 command;
	std::vector<std::string> params;

	std::pair<enum ComCategory, enum Commands> detectMsgType(const std::string& token);
};

// non-member functions

std::vector<Message> getMessages(const std::string& raw, Client *fd);

std::ostream& operator<<(std::ostream& os, const Message& msg);

// TODO: maybe move to utility file?
std::vector<std::string>
split(const std::string& str, const std::string& delimiter);

#endif // MESSAGE_HPP
