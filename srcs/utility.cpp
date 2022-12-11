#include <utility.hpp>

//diameter limited to len = 1
std::vector<std::string>
split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::string				 token;
	std::string				 s(str);
	size_t					 pos_del = s.find(delimiter);
	size_t					 pos_word_start = s.find_first_not_of(delimiter); //limits length of diameter to one char

	if (pos_word_start == std::string::npos)
	{
		return tokens;
	}
	else
	{
		s = s.substr(pos_word_start, s.size() - pos_word_start);
	}
	if (pos_del == std::string::npos)
	{
		tokens.push_back(s);
		return tokens;
	}
	while ((pos_del = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos_del);
		tokens.push_back(token);
		pos_word_start = s.find_first_not_of(delimiter, pos_del);
		if (pos_word_start == std::string::npos)
		{
			return tokens;
		}
		s.erase(0, pos_word_start);
	}
	if (!s.empty())
	{
		tokens.push_back(s);
	}
	return tokens;
}
