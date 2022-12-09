#include <Message.hpp>

std::string prefix::buildShortPrefix() const
{
	std::string result; // <servername>

	result = ":" + severname;

	return result;
}

std::string prefix::buildLongPrefix() const
{
	std::string result; // :<nick> [ '!' <user ] [ '@' <host> ] <SPACE>

	if (nick.empty())
		return "";
	result += ":" + nick;
	if (!user.empty())
		result += "!" + user;
	if (!host.empty())
		result += "@" + host;
	if (!result.empty())
		result += " ";
	return result;
}

Message::Message(const std::string& raw, Client* sending_client) : prefix()
{
	std::string tmp(raw);
	std::size_t found = raw.find(":", 1);
	if (found == std::string::npos || found == 0)
		this->text = "";
	else
	{
		this->text = raw.substr(found + 1);
		tmp = raw.substr(0, found);
	}
	std::vector<std::string> tokens = split(tmp, " "); 
	this->command = tokens[0];
	tokens.erase(tokens.begin());
	this->params = tokens;
	this->setSender(sending_client);
}

Message::Message(
	const std::vector<std::string>& parameters,
	Client*							sending_client)
	: params(parameters)
{
	// this->setCommand(cmd_type);
	this->setSender(sending_client);
}

Message::~Message() {}

// Getters

const struct prefix& Message::getPrefix() const
{
	return prefix;
}

std::string			Message::getText() const
{
	return (text);
}

// enum ComCategory Message::getComCategory() const
// {
// 	return category;
// }

// enum Commands Message::getType() const
// {
// 	return type;
// }

const std::string& Message::getCommand() const
{
	return command;
}

const std::vector<std::string>& Message::getParams() const
{
	return params;
}

Client* Message::getSender() const
{
	return sender;
}

// Setters

// void Message::setCommand(enum Commands cmd_type, const std::string& cmd_str)
// {
// 	if (!cmd_str.empty())
// 	{
// 		std::pair<enum ComCategory, enum Commands> cmd_cat_type =
// 			detectMsgType(cmd_str);
// 		// sanity check
// 		if (cmd_cat_type.second != cmd_type)
// 		{
// 			throw std::logic_error(
// 				"Command String does not match the Command number!");
// 		}
// 		this->command = cmd_str;
// 	}
// 	this->type = cmd_type;
// 	this->category = static_cast<enum ComCategory>(cmd_type / 10);
// 	// sanity check
// 	if (Message::commandMap.empty())
// 	{
// 		throw std::runtime_error("Command map not intitalized!");
// 	}
// 	this->command = Message::getCommandStr(cmd_type);
// }

/**
 * @brief copies the params into the Message class
 *
 */
void Message::setParams(const std::vector<std::string>& parameters)
{
	this->params = parameters; // TODO: verify that this is a copy
}

void Message::setSender(Client* sending_client)
{
	this->sender = sending_client;
	if (!sending_client)
		return;
	prefix.nick = sending_client->getNickname();
	prefix.user = sending_client->getUsername();
	prefix.host = "localhost";
	prefix.severname = "IRC";
}

std::string Message::buildRawMsg() const
{
	std::string msg;
	// if (category == RESPONSE)
	msg += prefix.buildShortPrefix();
	// else
	// {
	// 	msg += prefix.buildLongPrefix();
	// 	msg += command;
	// }

	// TODO: care about parameters that include spaces
	for (std::vector<std::string>::const_iterator it = params.begin();
		 it != params.end();
		 ++it)
	{
		msg += " ";
		msg += *it;
	}
	msg += "\r\n";
	return msg;
}

std::vector<Message> getMessages(const std::string& raw, Client* sender)
{
	std::vector<Message>	 messages;
	std::vector<std::string> raw_messages;
	const std::string		 msg_delimiter = "\r\n";

	std::cout << "Messages: " << std::endl; // DEBUG
	raw_messages = split(raw, msg_delimiter);
	for (std::vector<std::string>::const_iterator it = raw_messages.begin();
		 it != raw_messages.end();
		 ++it)
	{
		messages.push_back(Message(*it, sender)); 
	}
	return messages;
}
