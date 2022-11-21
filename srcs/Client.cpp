#include "Client.hpp"

Client::Client(int index, int cl_fd): id(index), client_fd(cl_fd), nickname(), state(LOCKED)
{}

Client::Client(): nickname(), state(LOCKED)
{}

Client::Client(const Client &c): client_fd(c.client_fd)
{}

Client::~Client()
{}

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
	if (this->state == LOCKED)
		command.compare("PASS");
	std::cout << "Command for <" << "> was: " << command << std::endl;
	return (0);
}
