// https://datatracker.ietf.org/doc/html/rfc1459#section-4.1
enum ComsInit
{
	// Parameters: <password>
	PASS,
	// Parameters: <nickname> [ <hopcount> ]
	NICK,
	// Parameters: <username> <hostname> <servername> <realname>
	USER
};

enum ComsMsg
{
	// Send private messages between users. <receiver> is the
	// nickname of the receiver of the message. <receiver> can also
	// be a list of names or channels separated with commas.
	// Parameters: <receiver>{,<receiver>} <text to be sent>
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.4.1
	PRIVMSG,
	// The difference between NOTICE and PRIVMSG is that automatic
	// replies must never be sent in response to a NOTICE message
	// Parameters: <nickname> <text>
	// https://datatracker.ietf.org/doc/html/rfc1459#section-4.4.2
	NOTICE
};

enum ComsOper
{
	KICK, 				// Eject a client from the channel
	MODE,				// Change the channel's mode
	INVITE,				// Invite a client to an invite-only channel (mode +i)
	TOPIC				//Change the channel topic in a mode +t channel

};

// https://datatracker.ietf.org/doc/html/rfc1459#section-4.6
enum ComsMisc
{
	// Available to operators and servers
	KILL,
	RESTART
};

// all command categories
enum ComCategory
{
	INIT,
	MSG,
	OPER,
	MISC
};

// // all supported commmands
// enum Coms
// {
// 	NONE = 0,
// 	PASS,
// 	NICK,
// 	USER,
// 	PRIVMSG = 10,
// 	NOTICE,
// 	KICK = 20,
// 	MODE,
// 	INVITE,
// 	TOPIC,
// 	KILL = 30,
// 	RESTART
// };
