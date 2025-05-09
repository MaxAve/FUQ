#include <utils.hpp>

bool db::utils::is_int(std::string str)
{
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] < '0' || str[i] > '9')
		{
			return false;
		}
	}
	return true;
}

bool db::utils::is_float(std::string str)
{
	bool dot_found = false;
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == '.')
		{
			if(dot_found)
			{
				return false;
			}
			dot_found = true;
		} else if(str[i] < '0' || str[i] > '9')
		{
			return false;
		}
	}
	return dot_found;
}

bool db::utils::is_number(std::string str)
{
	return db::utils::is_int(str) || db::utils::is_float(str);
}

bool db::utils::is_greater(std::string a, std::string b)
{
	if(db::utils::is_number(a) && db::utils::is_number(b))
	{
		if(db::utils::is_float(a) || db::utils::is_float(b))
		{
			return std::stof(a) > std::stof(b);
		}
		else
		{
			return std::stoll(a) > std::stoll(b);
		}
	}
	else
	{
		return std::tolower(a[0]) > std::tolower(b[0]); // TODO
	}
}