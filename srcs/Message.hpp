#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <Client.hpp>
#include <Commands.hpp>
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

// forward declaration to break include cycle
class Client;

class Message
{
public:
	Message();
	Message(const std::string& rawMsg, Client* sender);
	// Message(const Message& other);
	~Message();

	// Message& operator=(const Message& other);

	const struct prefix&			getPrefix() const;
	enum ComCategory				getComCategory() const;
	enum Commands					getType() const;
	const std::string&				getCommand() const;
	const std::vector<std::string>& getParams() const;
	Client*							getSender() const;

	void setCommand(enum Commands cmd, const std::string& cmd_str = "");
	void setParams(const std::vector<std::string>& params);
	void setSender(Client* sender);

	std::string buildRawMsg() const;

	static std::map< std::string, std::pair<enum ComCategory, enum Commands> >
		commandMap;
	static std::map< std::string, std::pair<enum ComCategory, enum Commands> >
	createCommandMap();

	friend std::ostream& operator<<(std::ostream& os, const Message& msg);
//temporary
public:
	struct prefix			 prefix; // empty in msg from client to server
	enum ComCategory		 category;
	enum Commands			 type;
	std::string				 command;
	std::vector<std::string> params;
	Client*					 sender; // NULL in msg from server to client


	// string -> enum (e.g. PASS -> 0)
	static std::pair<enum ComCategory, enum Commands>
	detectMsgType(const std::string& token);

	// enum -> string (e.g. 0 -> PASS)
	static const std::string getCommandStr(enum Commands cmd_type);

	static const std::string getCommandCategoryStr(enum ComCategory cmd_category);
};

// non-member functions

std::vector<Message> getMessages(const std::string& raw, Client* sender);

std::ostream& operator<<(std::ostream& os, const Message& msg);

#endif // MESSAGE_HPP
