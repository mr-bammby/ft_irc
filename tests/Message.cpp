#include <Message.hpp>

void str_to_msg(const std::string& s)
{
	Message msg(s);

	std::cout << msg << std::endl;
	std::cout << s << std::endl;
	std::cout << "---" << std::endl;
}

int main()
{
	// needs to be called before the first parser (Message constuctor) call
	Message::commandMap = Message::createCommandMap();

	Message emptyMsg;

	std::cout << "emptyMsg:" << emptyMsg << std::endl;

	// parse Messages
	// str_to_msg(""); // empty && no CRLF
	// str_to_msg("PASS pass"); // no CRLF
	str_to_msg("PASS pass\r\n");
	str_to_msg("USER chelmerd * * Chris\r\n");
	str_to_msg("USER chelmerd * * :Chris Helmerding\r\n");
	str_to_msg("NICK SuperDulli\r\n");
	str_to_msg("JOIN #foobar\r\n");			// join channel #foobar
	str_to_msg("JOIN &foo fubar\r\n");		// join channel &foo using key fubar
	str_to_msg("JOIN #foo,&bar fubar\r\n"); // join channel #foo using key fubar
											// and &bar using no key
	str_to_msg(
		"JOIN #foo,&bar fubar,foobar\r\n"); // join channel #foo using key fubar
											// and &bar using key foobar
	str_to_msg("JOIN #foo,&bar\r\n");		// join channels #foo and &bar
	// TODO: test other messages
	str_to_msg("PING :randomnbr");

	// build Message

	// Chris want to sent Alice a msg, server relays it to her with this
	Message					 response;
	std::vector<std::string> responseParams;
	response.setCommand(PRIVMSG);
	// response.setCommand(PRIVMSG, "PRIVMSG");
	// response.setCommand(PRIVMSG, "PASS"); // error
	responseParams.push_back("Alice");
	responseParams.push_back(":Hello World!");
	response.setParams(responseParams);

	Client client;
	client.upgradeState(); // bypass auth - lol
	client.upgradeState();
	client.setUsername("chelmerd");
	client.setNickname("SuperDulli");
	response.setSender(&client);
	std::cout << response << std::endl;

	// use this for sending
	const std::string rawResponse = response.buildRawMsg();

	// faster way of building a response
	responseParams[1] = "Bye";
	Message response2(PRIVMSG, responseParams, &client);
	std::cout << response2 << std::endl;

	// build an error response
	// TODO: this seems weird - for errors the server is consinderd the sender
	Client					 server;
	std::vector<std::string> errorParams;
	errorParams.push_back("461");
	errorParams.push_back("SupperDulli");
	errorParams.push_back("PRIVMSG");
	errorParams.push_back(":Not enough parameters");
	Message error(ERROR_RESPONSE, errorParams, &server);
	std::cout << error << std::endl;

	// build an cmd response like welcome etc.
	std::vector<std::string> cmdResponseParams;
	cmdResponseParams.push_back("001");
	cmdResponseParams.push_back("SuperDulli");
	cmdResponseParams.push_back(":Welome to our own IRC Server");
	cmdResponseParams.push_back("<more info>");
	Message cmdResponse(CMD_RESPONSE, cmdResponseParams, &server);
	std::cout << cmdResponse << std::endl;
}
