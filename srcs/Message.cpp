#include <Message.hpp>

std::string prefix::buildRawPrefix() const
{
	std::string result; // <nick> [ '!' <user ] [ '@' <host> ] <SPACE>

	if (nick.empty())
		return "";
	result += user;
	if (!user.empty())
		result += "!" + user;
	if (!host.empty())
		result += "@" + host;
	if (!result.empty())
		result += " ";
	return result;
}

Message::Message() : prefix(), category(MISC), type(-1), command(), params() {}

Message::Message(const std::string& raw) : prefix()
{
	std::vector<std::string> tokens = split(raw, " ");
	// TODO: consume tokens to fill Message
	std::pair<enum ComCategory, int> msgType = detectMsgType(tokens[0]);
	std::cout << "msgType=" << msgType.first << ", " << msgType.second
			  << std::endl;
	this->category = msgType.first;
	this->type = msgType.second;
	this->command = tokens[0];
	tokens.erase(tokens.begin());
	this->params = tokens;
	// TODO: check if the params match the type
}

Message::~Message() {}

std::string Message::buildRawMsg() const
{
	std::string msg = prefix.buildRawPrefix() + command + " ";
	// TODO: care about parameters that include spaces
	for (std::vector<std::string>::const_iterator it = params.begin();
		 it != params.end();
		 ++it)
	{
		msg += *it;
	}
	msg += "\r\n";
	return msg;
}

std::pair<enum ComCategory, enum Commands>
Message::detectMsgType(const std::string& token)
{
	const std::string init_commands[] = {"PASS", "NICK", "USER"};
	const size_t	  init_commands_len = 3;
	const std::string msg_commands[] = {"PRIVMSG", "NOTICE"};
	const size_t	  msg_commands_len = 2;
	const std::string oper_commands[] = {"KICK", "MODE", "INVITE", "TOPIC"};
	const size_t	  oper_commands_len = 4;
	const std::string misc_commands[] = {"KILL", "RESTART"};
	const size_t	  misc_commands_len = 2;

	// TODO: build map only once
	typedef std::map< std::string, std::pair<enum ComCategory, enum Commands> >
			   comMapType;
	comMapType cmd_map;
	for (size_t i = 0; i < init_commands_len; i++)
		cmd_map.insert(std::make_pair(
			init_commands[i],
			std::make_pair(INIT, static_cast<Commands>(INIT + i))));
	for (size_t i = 0; i < msg_commands_len; i++)
		cmd_map.insert(std::make_pair(
			msg_commands[i],
			std::make_pair(MSG, static_cast<Commands>(MSG + i))));
	for (size_t i = 0; i < oper_commands_len; i++)
		cmd_map.insert(std::make_pair(
			oper_commands[i],
			std::make_pair(OPER, static_cast<Commands>(OPER + i))));
	for (size_t i = 0; i < misc_commands_len; i++)
		cmd_map.insert(std::make_pair(
			misc_commands[i],
			std::make_pair(MISC, static_cast<Commands>(MISC + i))));

	comMapType::iterator it = cmd_map.find(token);
	if (it == cmd_map.end())
	{
		std::cerr << "Command not supported:" << token << std::endl;
		return std::make_pair(
			MISC,
			static_cast<Commands>(-1)); // TODO: seperate category for error?
	}
	return it->second;
}

// non-member functions

/**
 * @brief Extracts the Messages from the raw contents that the server receives.
 * Sometimes the server receives more than one message from the same client at
 * once
 *
 * @param raw
 * @return std::vector<Message>
 */
std::vector<Message> getMessages(const std::string& raw)
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
		messages.push_back(Message(*it)); // TODO: avoid construct empty Msg
	}
	return messages;
}

std::ostream& operator<<(std::ostream& os, const Message& msg)
{
	os << "Message(" << std::endl
	   << "prefix=" << msg.prefix.buildRawPrefix() << std::endl
	   << "category=" << msg.category << "(";
	switch (msg.category)
	{
	case INIT:
		os << "INIT";
		break;
	case MSG:
		os << "MSG";
		break;
	case OPER:
		os << "OPER";
		break;
	case MISC:
		os << "MISC";
		break;
	default:
		os << "UNKOWN";
		break;
	}
	os << ")" << std::endl;
	os << "type=" << msg.type << std::endl
	   << "params=" << msg.params << std::endl
	   << ") // Message" << std::endl;
	os << msg.buildRawMsg();
	return os;
}

std::vector<std::string>
split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::string				 token;
	std::string				 s(str);
	size_t					 pos = 0;

	// TODO: see what happens if the delimiter is not there
	// or appears more than once

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		tokens.push_back(token);
	}
	if (!s.empty())
		tokens.push_back(s);
	return tokens;
}
