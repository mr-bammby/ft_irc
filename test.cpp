#include "srcs/ServerResponse.hpp"

int main(void)
{
	Message::commandMap = Message::createCommandMap();

	Client client;
	client.upgradeState(); // bypass auth - lol
	client.upgradeState();
	client.setUsername("chelmerd");
	client.setNickname("SuperDulli");

	std::string test = Reply::welcome(&client);

	std::cout << test << std::endl; 

	test = Error::erroneousnickname("bitch");

	std::cout << test << std::endl; 

	test = Error::norecipient(&client, "HI");

	std::cout << test << std::endl; 

	test = Error::notexttosend(&client);

	std::cout << test << std::endl; 

	test = Error::unknowncommand(&client, "HI");

	std::cout << test << std::endl; 

	test = Error::nicknameinuse("bitch");

	std::cout << test << std::endl;

	test = Error::needmoreparams("HI");

	std::cout << test << std::endl; 

	test = Error::notregistered();

	std::cout << test << std::endl; 

	test = Error::passwdmismatch();

	std::cout << test << std::endl; 

	test = Error::nopriveleges(&client);

	std::cout << test << std::endl; 
}