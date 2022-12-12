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

Message::Message()
	: prefix(), category(MISC), type(static_cast<enum Commands>(-1)), command(),
	  params(), sender(NULL)
{
	commandMap = createCommandMap();
}


// Message::Message(const std::string& raw, Client *cl) : prefix(), client(cl)

Message::Message(const std::string& raw, Client* sending_client)
{
	std::string last_param;
	std::string raw_cpy(raw);

	#if MESSAGE_DUBG 
		std::cout<< "RAW: "<< raw <<std::endl;
	#endif
	find_prefx(&raw_cpy, &client_prefix); // finds and removes prefix
	find_last_param(&raw_cpy, &last_param); // finds and removes last parameter
	std::vector<std::string> tokens = split(raw_cpy, " "); //split command and reminding parameters
	#if MESSAGE_DUBG 
		if (tokens.empty())
			throw std::runtime_error("No CRLF in Message found!");
	#endif
	std::pair<enum ComCategory, enum Commands> msgType = detectMsgType(tokens[0]); //detects message type from command, command is always first "parameter"
	#if MESSAGE_DUBG 
		std::cout << "msgType=" << msgType.first << ", " << msgType.second
				<< std::endl;
	#endif
	this->category = msgType.first;
	this->type = msgType.second;
	this->command = tokens[0];
	tokens.erase(tokens.begin());
	this->params = tokens;
	if (last_param.size())
		this->params.push_back(last_param);
	this->setSender(sending_client);
}

Message::Message(
	enum Commands					cmd_type,
	const std::vector<std::string>& parameters,
	Client*							sending_client)
	: params(parameters)
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

//Dont use getText unsless you want to return text
// Adds spaces between params
std::string Message::getText() const
{
	std::string result;
	for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); it++)
	{
		result = result + *it;
		if (it + 1 != params.end())
			result += " ";
	}
	return (result);
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
	prefix.host = "localhost";
	prefix.severname = "IRC";
}

std::string Message::buildRawMsg() const
{
	std::string msg;
	if (category == RESPONSE)
		msg += prefix.buildShortPrefix();
	else
	{
		msg += prefix.buildLongPrefix();
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

std::map< std::string, std::pair<enum ComCategory, enum Commands> >
	Message::commandMap;

std::map< std::string, std::pair<enum ComCategory, enum Commands> >
Message::createCommandMap()
{
	const std::string init_commands[] = {"PASS", "NICK", "USER"};
	const size_t	  init_commands_len = 3;
	const std::string msg_commands[] = {"PRIVMSG", "NOTICE"};
	const size_t	  msg_commands_len = 2;
	const std::string channel_commands[] = {"KICK", "MODE", "INVITE", "TOPIC", "PART", "WHO", "NAMES", "LIST", "JOIN"};
	const size_t	  channel_commands_len = 9;
	const std::string response_commands[] = {"CMD_RESPONSE", "ERROR_RESPONSE"};
	const size_t	  response_commands_len = 2;
	const std::string ignore_commands[] = {"PING"};
	const size_t	  ignore_commands_len = 1;
	const std::string misc_commands[] = {"KILL", "RESTART", "OPER", "QUIT", "SQUIT"};
	const size_t	  misc_commands_len = 5;

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
	for (size_t i = 0; i < channel_commands_len; i++)
		cmd_map.insert(std::make_pair(
			channel_commands[i],
			std::make_pair(CHANNEL, static_cast<Commands>(CHANNEL * 10 + i))));
	for (size_t i = 0; i < response_commands_len; i++)
		cmd_map.insert(std::make_pair(
			response_commands[i],
			std::make_pair(
				RESPONSE, static_cast<Commands>(RESPONSE * 10 + i))));
	for (size_t i = 0; i < ignore_commands_len; i++)
		cmd_map.insert(std::make_pair(
			ignore_commands[i],
			std::make_pair(IGNORE, static_cast<Commands>(IGNORE * 10 + i))));
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
			static_cast<Commands>(UNKNOWN)); // TODO: seperate category for error?
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
	case PING:
		return "PING";
	case KILL:
		return "KILL";
	case RESTART:
		return "RESTART";
	case OPER:
		return "OPER";
	case QUIT:
		return "QUIT";
	case PART:
		return "PART";
	case WHO:
		return "WHO";
	case NAMES:
		return "NAMES";
	case SQUIT:
		return "SQUIT";
	case LIST:
		return "LIST";
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
	case CHANNEL:
		return "CHANNEL";
	case RESPONSE:
		return "RESPONSE";
	case IGNORE:
		return "IGNORE";
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
		messages.push_back(Message(*it, sender)); 
	}
	return messages;
}

std::vector<Message> getMessages(const std::string& raw, Client* sender, \
std::map<Client*, std::string> &incomplete_map)
{
	std::vector<Message>	 messages;
	std::vector<std::string> raw_messages;
	const std::string		 msg_delimiter = "\r\n";


	raw_messages = split(raw, msg_delimiter);
	// If part of previous input that was unfinished
	if (incomplete_map.find(sender) != incomplete_map.end())
	{
		std::cout << GR << "Incomplete fella found" << std::endl;
		std::string		temp = incomplete_map[sender];
		if (raw.find(msg_delimiter) == std::string::npos) // KEEP ADDING
		{
			for (std::vector<std::string>::const_iterator it = raw_messages.begin();
			it != raw_messages.end(); ++it)
				incomplete_map[sender].append(*it);
			return (messages);
		}
		else			// IS FINISHED
		{
			std::cout << GR << "Has been finished" << std::endl;
			//Changed here, because if we write full working command, for example "PASS 12" and press ctrl + D, 
			// then press enter without any chars, it segfaulted, because raw_messages[0] didnt exist
			if (raw.find("\r\n") == 0)
				raw_messages.insert(raw_messages.begin(), temp);
			else
				raw_messages[0].insert(0, temp);
			incomplete_map.erase(sender);
		}		
	}
	else if (raw.find(msg_delimiter) == std::string::npos)	// If new user's incomplete message
	{
		incomplete_map[sender] = raw_messages[0];
		return (messages);
	}
	for (std::vector<std::string>::const_iterator it = raw_messages.begin();
		 it != raw_messages.end();
		 ++it)
	{
		messages.push_back(Message(*it, sender)); 
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
	   << "params=" << msg.params[0] << std::endl
	   << ") // Message" << std::endl;
	os << msg.buildRawMsg();
	return os;
}


void Message::find_prefx(std::string *raw, std::string *prefix)
{
	std::size_t first_char_index = raw->find_first_not_of(" ");
	std::size_t end_of_prefix_index; 

	if (first_char_index != std::string::npos && first_char_index != 0)
	{
		*raw = raw->substr(first_char_index); // remows leading spaces
	}
	if ((*raw)[0] == ':')
	{
		end_of_prefix_index = raw->find_first_of(" ");
		if (end_of_prefix_index == std::string::npos)
		{
			*prefix = raw->substr(1);
			*raw = "";
		}
		else
		{
			*prefix = raw->substr(1, end_of_prefix_index - 1);
			*raw = raw->substr(end_of_prefix_index, raw->size() - end_of_prefix_index);
		}
	}
	else
	{
		*prefix = "";
	}
}

void  Message::find_last_param(std::string *raw, std::string *last_param)//must be called after find_prefix
{
	std::string delimeter = ":"; //change string to locate to " :" in needed
	std::size_t delemiter_index = raw->find(delimeter);

	if (delemiter_index != std::string::npos)
	{
		if ((raw->size() - delemiter_index) > delimeter.size())
		{
			*last_param = raw->substr(delemiter_index + delimeter.size(), raw->size() - (delemiter_index + delimeter.size())); // removes leading spaces
		}
		else
		{
			*last_param = "";
		}
		*raw = raw->substr(0, delemiter_index);
	}
	else
	{
		*last_param = "";
	}
}
