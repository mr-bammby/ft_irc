#include "Client.hpp"

Client::Client(int index, int cl_fd)
	: id(index), client_fd(cl_fd), nickname(), state(LOCKED)
{}

Client::Client() : nickname(), state(LOCKED) {}

Client::Client(const Client& c) : client_fd(c.client_fd) {}

Client::~Client() {}

int Client::setNickname(std::string name)
{
	if (state == LOCKED)
	{
		return (-1);
	}
	nickname = name;
	if (UNINITIALIZED)
	{
		state = INITIALIZED;
	}
	return (0);
}

const std::string& Client::getNickname()
{
	return (nickname);
}

const Client::State& Client::getState()
{
	return (state);
}

int Client::parse(std::string command)
{
	// sometimes the server receives more than one message from the same client
	// at a time
	std::vector<std::string> messages;
	const std::string		 msg_delimiter = "\r\n";
	const std::string		 param_delimiter = " ";

	std::cout << "Messages: " << std::endl; // DEBUG
	messages = split(command, msg_delimiter);

	std::cout << "Tokens: " << std::endl; // DEBUG
	for (std::vector<std::string>::const_iterator it = messages.begin();
		 it != messages.end();
		 ++it)
	{
		std::vector<std::string> tokens = split(*it, " ");
		// TODO: consume tokens to fill struct Message
		std::pair<enum ComCategory, int> msgType = detectMsgType(tokens[0]);
		std::cout << "mstType=" << msgType.first << ", " << msgType.second
				  << std::endl;
	}

	if (this->state == LOCKED)
		command.compare("PASS");
	std::cout << "Command for <"
			  << "> was: " << command << std::endl;
	return (0);
}

std::vector<std::string>
Client::split(const std::string& str, const std::string& delimiter)
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
		std::cout << token << ", ";
	}
	tokens.push_back(s); // TODO: avoid pushing "empty"/only delimiters
	std::cout << s << "." << std::endl;
	return tokens;
}

std::pair<enum ComCategory, int> Client::detectMsgType(const std::string& token)
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
	typedef std::map< std::string, std::pair<enum ComCategory, int> >
			   comMapType;
	comMapType cmd_map;
	for (size_t i = 0; i < init_commands_len; i++)
		cmd_map.insert(
			std::make_pair(init_commands[i], std::make_pair(INIT, i)));
	for (size_t i = 0; i < msg_commands_len; i++)
		cmd_map.insert(std::make_pair(msg_commands[i], std::make_pair(MSG, i)));
	for (size_t i = 0; i < oper_commands_len; i++)
		cmd_map.insert(
			std::make_pair(oper_commands[i], std::make_pair(OPER, i)));
	for (size_t i = 0; i < misc_commands_len; i++)
		cmd_map.insert(
			std::make_pair(misc_commands[i], std::make_pair(MISC, i)));

	comMapType::iterator it = cmd_map.find(token);
	if (it == cmd_map.end())
	{
		std::cerr << "Command not supported:" << token << std::endl;
		return std::make_pair(MISC, -1); // TODO: seperate category for error?
	}
	return it->second;
}
