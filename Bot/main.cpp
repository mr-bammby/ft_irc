#include "Bot.hpp"

int main(int argc, char *argv[])
{
	if (argc == 3)
	{
		for (int i = 0; argv[1][i]; i++)
		{
			if (std::isdigit(argv[1][i]) == 0)
			{
				std::cout<<"Port should be a number"<<std::endl;
				return (0);
			}
		}
		int port;
		sscanf(argv[1], "%d", &port);
		{
			Bot bot(port, argv[2]);
			bot.run();
		}
	}
	else
	{
		std::cout << "./bot port password" << std::endl;
	}
	return (0);
}
