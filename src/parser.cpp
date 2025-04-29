#include <parser.hpp>

token_t db::parser::get(std::vector<token_t> tokens, int i)
{
	if(i >= tokens.size() || i < 0)
		return "";
	return tokens[i];
}

std::string db::parser::remove_comments(std::string str)
{
	bool in_string = false;
	for(int i = 0; i < str.length(); i++)
	{
		if(i == 0 && str[i] == '"')
			in_string = !in_string;
		else if(str[i] == '"' && str[i - 1] != '\\')
			in_string = !in_string;

		if(!in_string && str[i] == '#')
		{
			return str.substr(0, i);
		}
	}
	return str;
}

std::string db::parser::filter(std::string str, char c, bool ignore_strings)
{
	std::string res = "";
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == c)
			continue;
		res += str[i];
	}
	return res;
}

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

std::string db::parser::get_operator(std::string str, int i)
{
	std::string res = "";
	for(auto& it : db::script::operators)
		if((i + it.first.length()) <= str.length())
			if(str.substr(i, it.first.length()) == it.first)
				res = it.first;
	return res;
}

std::vector<token_t> db::parser::tokenize(std::string str)
{
	str = db::parser::remove_comments(str);

	std::vector<token_t> res = {""};

	for(int i = 0; i < str.size(); i++)
	{
		if(str[i] == '\'' || str[i] == '"')
		{
			std::string str_token = "\"";
			for(int j = i + 1; j < str.size(); j++)
			{
				if(str[j] == '\'' || str[j] == '"')
				{
					i = j;
					break;
				}
				str_token += str[j];
			}
			res[res.size() - 1] = str_token + "\"";
			res.push_back("");
			continue;
		}
		token_t op = db::parser::get_operator(str, i);
		if(op.length() > 0)
		{
			if(res[res.size() - 1].length() > 0)
			{
				res.push_back(op);
				res.push_back("");
			}
			else
			{
				res[res.size() - 1] = op;
				res.push_back("");
			}
			i += op.length() - 1;
			if(str[i + 1] == ' ')
				i++;
			continue;
		}
		res[res.size() - 1] += str[i];
	}

	if(res[res.size() - 1].length() == 0)
		res.pop_back();

	return res;
}
