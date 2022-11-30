/*
	You must be able to authenticate, set a nickname, a username, join a
   channel, send and receive private messages using your reference client. All
   the messages sent from one client to a channel have to be forwarded to every
   other client that joined the channel. You must have operators and regular
   users. Then, you have to implement the commands that are specific to
   operators
*/
#ifndef COMMANDS_HPP
# define COMMANDS_HPP
// https://datatracker.ietf.org/doc/html/rfc1459#section-4.1
enum Commands
{
	// Parameters: <password>
	PASS,
	// Parameters: <nickname> [ <hopcount> ]
	NICK,
	// Parameters: <username> <hostname> <servername> <realname>
	USER,
	// Parameters: <channel>{,<channel>} [<key>{,<key>}]
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.1
	JOIN,
	// Send private messages between users. <receiver> is the
	// nickname of the receiver of the message. <receiver> can also
	// be a list of names or channels separated with commas.
	// Parameters: <receiver>{,<receiver>} <text to be sent>
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.4.1
	PRIVMSG = 10,
	// The difference between NOTICE and PRIVMSG is that automatic
	// replies must never be sent in response to a NOTICE message
	// Parameters: <nickname> <text>
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.4.2
	NOTICE,
	// Operator Commands
	KICK = 20, // Eject a client from the channel
	MODE,	   // Change the channel's mode
	INVITE,	   // Invite a client to an invite-only channel (mode +i)
	TOPIC,	   // Change the channel topic in a mode +t channel
	// responses diffrent from other commands: no command after prefix
	// 200 -> 3XX
	CMD_RESPONSE = 30,
	// 400 -> 5XX
	ERROR_RESPONSE,
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.6
	// Available to operators and servers
	PING = 40,
	KILL = 50,
	RESTART
};

// all command categories
enum ComCategory
{
	INIT,
	MSG,
	OPER,
	RESPONSE,
	IGNORE,
	MISC
};
#endif
