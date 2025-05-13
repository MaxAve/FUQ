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

std::vector<std::vector<std::string>> db::utils::quicksort_table(std::vector<std::vector<std::string>> table, int column)
{
	std::vector<std::vector<std::string>> less, equal, greater;

	if(table.size() > 1)
	{
		std::string pivot = table[0][column];

		for(int i = 0; i < table.size(); i++)
		{
			if(db::utils::is_greater(table[i][column], pivot))
				greater.push_back(table[i]);
			else if(table[i][column] == pivot)
				equal.push_back(table[i]);
			else
				less.push_back(table[i]);
		}

		// Join tables
		std::vector<std::vector<std::string>> res = db::utils::quicksort_table(less, column);
		res.insert(res.end(), equal.begin(), equal.end());
		greater = db::utils::quicksort_table(greater, column);
		res.insert(res.end(), greater.begin(), greater.end());

		return res;
	}
	else
	{
		return table;
	}
}