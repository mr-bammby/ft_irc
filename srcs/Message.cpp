#include <Message.hpp>

std::string prefix::buildShortPrefix() const
{
	std::string result; // <servername>

	result = severname;

	return result;
}

std::string prefix::buildLongPrefix() const
{
	std::string result; // <nick> [ '!' <user ] [ '@' <host> ] <SPACE>

	if (nick.empty())
		return "";
	result += nick;
	if (!user.empty())
		result += "!" + user;
	if (!host.empty())
		result += "@" + host;
	if (!result.empty())
		result += " ";
	return result;
}

Message::Message()
	: prefix(), category(MISC), type(static_cast<enum Commands>(-1)), command(),
	  params(), sender(NULL)
{
	commandMap = createCommandMap();
}


// Message::Message(const std::string& raw, Client *cl) : prefix(), client(cl)

Message::Message(const std::string& raw, Client* sending_client) : prefix()
{
	std::vector<std::string> tokens = split(raw, " "); // TODO: sometimes params are allowed to have whitespace
	// TODO: indicate error when no CRLF was found
	std::cout<< "RAW: "<< raw <<std::endl;
	std::cout<< "Tokens: "<< tokens <<std::endl;
	if (tokens.empty())
		throw std::runtime_error("No CRLF in Message found!");
	// TODO: consume tokens to fill Message
	std::pair<enum ComCategory, int> msgType = detectMsgType(tokens[0]);
	std::cout << "msgType=" << msgType.first << ", " << msgType.second
			  << std::endl;
	this->category = msgType.first;
	this->type = static_cast<enum Commands>(msgType.second);
	this->command = tokens[0];
	tokens.erase(tokens.begin());
	this->params = tokens;
	this->setSender(sending_client);
	// TODO: check if the params match the type
}

Message::Message(enum Commands cmd_type,
		const std::vector<std::string>& parameters,
		Client*							sending_client) : params(parameters)
{
	this->setCommand(cmd_type);
	this->setSender(sending_client);
}

Message::~Message() {}

// Getters

const struct prefix& Message::getPrefix() const
{
	return prefix;
}

enum ComCategory Message::getComCategory() const
{
	return category;
}

enum Commands Message::getType() const
{
	return type;
}

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

void Message::setCommand(enum Commands cmd_type, const std::string& cmd_str)
{
	if (!cmd_str.empty())
	{
		std::pair<enum ComCategory, enum Commands> cmd_cat_type =
			detectMsgType(cmd_str);
		// sanity check
		if (cmd_cat_type.second != cmd_type)
		{
			throw std::logic_error(
				"Command String does not match the Command number!");
		}
		this->command = cmd_str;
	}
	this->type = cmd_type;
	this->category = static_cast<enum ComCategory>(cmd_type / 10);
	// sanity check
	if (Message::commandMap.empty())
	{
		throw std::runtime_error("Command map not intitalized!");
	}
	this->command = Message::getCommandStr(cmd_type);
}

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
	// TODO: gethostname dynamically
	prefix.host = "localhost";
	prefix.severname = "<servername>";
}

std::string Message::buildRawMsg() const
{
	std::string msg(":");
	if (category == RESPONSE)
		msg += prefix.buildShortPrefix();
	else
	{
		msg	+= prefix.buildLongPrefix();
		msg += command;
	}

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

// static member functions

std::map< std::string, std::pair<enum ComCategory, enum Commands> > Message::commandMap;

std::map< std::string, std::pair<enum ComCategory, enum Commands> >
Message::createCommandMap()
{
	const std::string init_commands[] = {"PASS", "NICK", "USER", "JOIN"};
	const size_t	  init_commands_len = 4;
	const std::string msg_commands[] = {"PRIVMSG", "NOTICE"};
	const size_t	  msg_commands_len = 2;
	const std::string oper_commands[] = {"KICK", "MODE", "INVITE", "TOPIC"};
	const size_t	  oper_commands_len = 4;
	const std::string response_commands[] = {"CMD_RESPONSE", "ERROR_RESPONSE"};
	const size_t	  response_commands_len = 2;
	const std::string misc_commands[] = {"KILL", "RESTART"};
	const size_t	  misc_commands_len = 2;

	// TODO: build similar map for nb# of parameters for a given cmd
	typedef std::map< std::string, std::pair<enum ComCategory, enum Commands> >
			   comMapType;
	comMapType cmd_map;
	for (size_t i = 0; i < init_commands_len; i++)
		cmd_map.insert(std::make_pair(
			init_commands[i],
			std::make_pair(INIT, static_cast<Commands>(INIT * 10 + i))));
	for (size_t i = 0; i < msg_commands_len; i++)
		cmd_map.insert(std::make_pair(
			msg_commands[i],
			std::make_pair(MSG, static_cast<Commands>(MSG * 10 + i))));
	for (size_t i = 0; i < oper_commands_len; i++)
		cmd_map.insert(std::make_pair(
			oper_commands[i],
			std::make_pair(OPER, static_cast<Commands>(OPER * 10 + i))));
	for (size_t i = 0; i < response_commands_len; i++)
		cmd_map.insert(std::make_pair(
			response_commands[i],
			std::make_pair(RESPONSE, static_cast<Commands>(RESPONSE * 10 + i))));
	for (size_t i = 0; i < misc_commands_len; i++)
		cmd_map.insert(std::make_pair(
			misc_commands[i],
			std::make_pair(MISC, static_cast<Commands>(MISC * 10 + i))));
	return cmd_map;
}

std::pair<enum ComCategory, enum Commands>
Message::detectMsgType(const std::string& token)
{
	// sanity check
	if (commandMap.empty())
	{
		throw std::runtime_error("Command map not intitalized!");
	}
	std::map< std::string, std::pair<enum ComCategory, enum Commands> >::
		iterator it = commandMap.find(token);
	if (it == commandMap.end())
	{
		std::cerr << "Command not supported:" << token << std::endl;
		return std::make_pair(
			MISC,
			static_cast<Commands>(-1)); // TODO: seperate category for error?
	}
	return it->second;
}

const std::string Message::getCommandStr(enum Commands cmd_type)
{
	switch (cmd_type)
	{
	case PASS:
		return "PASS";
	case NICK:
		return "NICK";
	case USER:
		return "USER";
	case JOIN:
		return "JOIN";
	case PRIVMSG:
		return "PRIVMSG";
	case NOTICE:
		return "NOTICE";
	case KICK:
		return "KICK";
	case MODE:
		return "MODE";
	case INVITE:
		return "INVITE";
	case TOPIC:
		return "TOPIC";
	case CMD_RESPONSE:
		return "CMD_RESPONSE";
	case ERROR_RESPONSE:
		return "ERROR_RESPONSE";
	case KILL:
		return "KILL";
	case RESTART:
		return "RESTART";
	default:
		return "UNKOWN";
	}
}

const std::string Message::getCommandCategoryStr(enum ComCategory cmd_category)
{
	switch (cmd_category)
	{
	case INIT:
		return "INIT";
	case MSG:
		return "MSG";
	case OPER:
		return "OPER";
	case RESPONSE:
		return "RESPONSE";
	case MISC:
		return "MISC";
	default:
		return "UNKOWN";
	}
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
		messages.push_back(Message(*it, sender)); // TODO: avoid construct empty Msg
	}
	return messages;
}

std::ostream& operator<<(std::ostream& os, const Message& msg)
{
	// TODO: use getters and remove friend
	os << "Message(" << std::endl
	   << "prefix=" << msg.prefix.buildLongPrefix() << std::endl
	   << "category=" << msg.category << "("
	   << Message::getCommandCategoryStr(msg.category) << ")" << std::endl
	   << "type=" << msg.type << "(" << Message::getCommandStr(msg.type) << ")"
	   << std::endl
	   << "params=" << msg.params << std::endl
	   << ") // Message" << std::endl;
	os << msg.buildRawMsg();
	return os;
}


