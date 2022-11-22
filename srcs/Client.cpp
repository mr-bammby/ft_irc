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
		// TODO: consume tokens
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

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		tokens.push_back(token);
		std::cout << token << ", ";
	}
	tokens.push_back(s);
	std::cout << s << "." << std::endl;
	return tokens;
}
