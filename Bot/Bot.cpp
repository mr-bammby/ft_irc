#include "Bot.hpp"
Bot::Bot(int port, std::string pass) : pass(pass)
{
	nick = "partybot";
	if ((bot_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);

	struct addrinfo bla, *serv;

	bla.ai_family = AF_UNSPEC;
	bla.ai_socktype = SOCK_STREAM;
	std::string host = "localhost";
	getaddrinfo(host.c_str(), 0, &bla, &serv);

	servAddr.sin_addr = ((struct sockaddr_in *)serv->ai_addr)->sin_addr;
	return ;
}
Bot::Bot(Bot const &src)
{
	*this = src;
	return ;
}
Bot::~Bot(void)
{
	send_msg("QUIT\r\n");
	return ;
}
// Bot	&Bot::operator=(Bot const &rhs)
// {
// 	return *this;
// }

void Bot::run()
{
	while (connect(bot_fd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		;
	join_server();
	while(awake == 1)
	{
		if (messages.empty())
			recieve_msg();
		if (messages[0].find("JOIN") != std::string::npos)
			send_msg("PRIVMSG #PARTY welcome to the party!!\r\n");
		else if (messages[0].find("party") != std::string::npos)
			send_msg("PRIVMSG #PARTY did someone say party????\r\n"); // i think these are where the segfault occurs 
		messages.erase(messages.begin());
	}
	messages.clear();
}

void Bot::send_msg(std::string msg)
{
	if (send(bot_fd, msg.c_str(), msg.size(), 0) < 0)
	{
		awake = 0;
		std::cout << nick << " unable to send message :(" << std::endl;
	}
}

void Bot::recieve_msg() //if no message recieved when func called
{
	char buf[1024];
	struct pollfd pfd;

	pfd.fd = bot_fd;
	pfd.events = POLLIN;
	if (poll(&pfd, 1, -1) == -1)
	{
		awake = 0;
		return;
		// if (pfd.revents & POLLERR)
		// {
			
		// }
	}
	ssize_t len = recv(bot_fd, &buf, 1024, 0);
	if (len <= 0)
	{
		awake = 0;
		return;
	}
	buf[len] = '\0';
	rawMessage += buf;
	while(rawMessage.find("\r\n") != std::string::npos && rawMessage.size() != 0)
	{
		std::string msg = rawMessage.substr(0, rawMessage.find("\r\n"));
		rawMessage.erase(0, rawMessage.find("\r\n") + 2);
		messages.push_back(msg);
	}
}

void Bot::join_server()
{
	send_msg("PASS " + pass + "\r\n");
	//recieve_msg(); fix
	if (messages.size() > 0)
	{
		std::cout << "wrong password" << std::endl;
		return ;
	}
	send_msg("USER " + nick + "\r\n");
	send_msg("NICK " + nick + "\r\n");
	recieve_msg();
	if (messages.size() > 2)
	{
		std::cout << "bot nickname in use" << std::endl;
		return;
	}
	awake = 1;
	send_msg("JOIN #PARTY\r\n");
	send_msg("TOPIC #PARTY partytime\r\n");
	messages.clear();
}