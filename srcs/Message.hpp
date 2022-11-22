#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <Commands.hpp>
#include <utility.hpp>

class Message
{
public:
	Message();
	Message(const std::string& rawMsg);
	// Message(const Message& other);
	~Message();

	// Message& operator=(const Message& other);

	std::string buildRawMsg() const;

	friend std::ostream& operator<<(std::ostream& os, const Message& msg);

private:
	// TODO: give prefix a structure
	std::string				 prefix; // empty in msg from client to server
	enum ComCategory		 category;
	int						 type;
	std::vector<std::string> params;

	std::pair<enum ComCategory, int> detectMsgType(const std::string& token);
};

// non-member functions

std::vector<Message> getMessages(const std::string& raw);

std::ostream& operator<<(std::ostream& os, const Message& msg);

// TODO: maybe move to utility file?
std::vector<std::string>
split(const std::string& str, const std::string& delimiter);

#endif // MESSAGE_HPP
