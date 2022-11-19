#include "Server.hpp"

int main(int argc, char *argv[])
{
	(void) argv;
	if (argc != 3)
	{
		Server s(6667, std::string("password1234"));
	}
	else
	{
		printf("You must run the Server with follwing commands:\n");
		printf("./ircserv <port> <password>\n");
	}
	return (0);
}