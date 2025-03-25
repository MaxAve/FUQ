#include <parser.hpp>

std::string db::parser::normalize(std::string str, char c, bool ignore_strings)
{
	std::string res = "";
	bool in_string = false;
	for(int i = 0; i < str.size(); i++)
	{
		if(ignore_strings && (str[i] == '"' || str[i] == '\''))
			in_string = !in_string;
		if(in_string)
		{
			res += str[i];
			continue;
		}
		if(str[i] == c)
		{
			for(int j = i; str[j] == c; j++)
				i = j;
		}
		res += str[i];
	}
	return res;
}

std::vector<std::string> db::parser::split(std::string str, char c)
{
	std::vector<std::string> res = {""};
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == c)
		{
			res.push_back("");
			continue;
		}
		res[res.size() - 1] += str[i];
	}
	return res;
}
