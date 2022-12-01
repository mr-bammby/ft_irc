#include <utility.hpp>

std::vector<std::string>
split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::string				 token;
	std::string				 s(str);
	size_t					 pos = s.find(delimiter);

	// TODO: see what happens if the delimiter is not there
	// or appears more than once
	if (pos == std::string::npos)
	{
		tokens.push_back(s);
		return tokens;
	}
	// TODO: dont split after :

	while ((pos = s.find(delimiter)) != std::string::npos && s[0] != ':')
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
		tokens.push_back(token);
	}
	if (!s.empty())
		tokens.push_back(s);
	return tokens;
}
