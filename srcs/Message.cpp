#include <Message.hpp>

std::string prefix::buildShortPrefix() const
{
	std::string result;

	result = ":" + severname;

	return result;
}

std::string prefix::buildLongPrefix() const
{
	std::string result;

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

Message::Message(const std::string& raw, Client* sending_client)
{
	std::string last_param;
	std::string raw_cpy(raw);

	find_prefx(&raw_cpy, &client_prefix);
	find_last_param(&raw_cpy, &last_param);
	std::vector<std::string> tokens = split(raw_cpy, " ");

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

void Message::setCommand(enum Commands cmd_type)
{
	this->type = cmd_type;
	this->category = static_cast<enum ComCategory>(cmd_type / 10);
	this->command = getCommandStr(cmd_type);
}

void Message::setParams(const std::vector<std::string>& parameters)
{
	this->params = parameters; 
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

std::string Message::getCommandStr(enum Commands cmd_type)
{
	switch (cmd_type)
	{
	case CMD_RESPONSE:
		return "CMD_RESPONSE";
	case ERROR_RESPONSE:
		return "ERROR_RESPONSE";
	default:
		return "UNKOWN";
	}
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
		std::string		temp = incomplete_map[sender];
		if (raw.find(msg_delimiter) == std::string::npos)
		{
			for (std::vector<std::string>::const_iterator it = raw_messages.begin();
			it != raw_messages.end(); ++it)
				incomplete_map[sender].append(*it);
			return (messages);
		}
		else
		{
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

void Message::find_prefx(std::string *raw, std::string *prefix)
{
	std::size_t first_char_index = raw->find_first_not_of(" ");
	std::size_t end_of_prefix_index; 

	if (first_char_index != std::string::npos && first_char_index != 0)
	{
		*raw = raw->substr(first_char_index);
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

void  Message::find_last_param(std::string *raw, std::string *last_param)
{
	std::string delimeter = ":"; 
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

std::vector<std::string>
split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::string				 token;
	std::string				 s(str);
	size_t					 pos_del = s.find(delimiter);
	size_t					 pos_word_start = s.find_first_not_of(delimiter); //limits length of diameter to one char

	if (pos_word_start == std::string::npos)
	{
		return tokens;
	}
	else
	{
		s = s.substr(pos_word_start, s.size() - pos_word_start);
	}
	if (pos_del == std::string::npos)
	{
		tokens.push_back(s);
		return tokens;
	}
	while ((pos_del = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos_del);
		tokens.push_back(token);
		pos_word_start = s.find_first_not_of(delimiter, pos_del);
		if (pos_word_start == std::string::npos)
		{
			return tokens;
		}
		s.erase(0, pos_word_start);
	}
	if (!s.empty())
	{
		tokens.push_back(s);
	}
	return tokens;
}
