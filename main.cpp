#include "Server.hpp"
#include "Comms.hpp"

int checkPort(char* argv)
{
	for (int i = 0; argv[i]; i++)
	{
		if (std::isdigit(argv[i]) == 0)
		{
			std::cout<<"Port should be a number"<<std::endl;
			return (0);
		}
	}
	int port;
	sscanf(argv, "%d", &port);
	if (port < 1025)
	{
		std::cout<<"Port should be a bigger then 1024"<<std::endl;
		return (0);
	}
	return (port);
}

int main(int argc, char *argv[])
{
	int port;

	if (argc == 3)
	{
		port = checkPort(argv[1]);
		if (!port)
			return (1);
		Server s(port, argv[2]);
		s.init();
		// Message::commandMap = Message::createCommandMap();
		while (s.on)
		{
			s.start_loop();
			s.executor();
		}
	}
	else
	{
		printf("You must run the Server with follwing commands:\n");
		printf("./ircserv <port> <password>\n");
	}
	return (0);
}
