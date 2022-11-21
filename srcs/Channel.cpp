#include "Channel.hpp"

Channel::Channel(std::string op): chanop(op)
{}

Channel::Channel(const Channel &c): name(c.name)
{}

Channel::~Channel()
{}

Channel &Channel::operator=(const Channel &c)
{
	this->name = c.name;
	return (*this);
}

int Channel::broadcast(std::string message)
{
	// int i = 0;
	std::cout << message << std::endl;
	// while ()
	return (0);
}
