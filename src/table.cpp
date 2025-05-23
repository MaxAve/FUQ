#include <table.hpp>

db::table::Table::Table(std::string file)
{
	this->sort_rule = {true, 0};

	if(file.substr(file.length() - 4, 4) == ".csv")
	{
		std::ifstream f(file);

		std::string line;

		int linenum = 0;

		if(f.is_open())
		{
			while(std::getline(f, line))
			{
				linenum++;

				this->table.push_back(std::vector<std::string>());
				this->table[this->table.size() - 1].push_back("");
				
				for(int i = 0; i < line.length(); i++)
				{
					if(line[i] == '\\')
					{
						if(i == line.length() - 1)
						{
							std::cout << "ERROR: (While trying to open " << file << ") Expected escape sequence character after \\ at line " << linenum << "\n";
							return;
						}
						if(line[i + 1] == '\\')
						{
							this->table[this->table.size() - 1][this->table[this->table.size() - 1].size() - 1] += '\\';
							i++;
							continue;
						}
						else if(line[i + 1] == ',')
						{
							this->table[this->table.size() - 1][this->table[this->table.size() - 1].size() - 1] += ',';
							i++;
							continue;
						}
					}

					if(line[i] == ',')
					{
						this->table[this->table.size() - 1].push_back("");
						continue;
					}
					this->table[this->table.size() - 1][this->table[this->table.size() - 1].size() - 1] += line[i];
				}

				if(this->table[this->table.size() - 1][this->table[this->table.size() - 1].size() - 1].length() == 0)
				{
					this->table[this->table.size() - 1].pop_back();
				}
				
				if(this->table.size() > 1 && this->table[this->table.size() - 1].size() != this->table[0].size())
				{
					std::cout << "ERROR: (While trying to open " << file << ") Could not parse line " << linenum << " because the number of columns (" << this->table[this->table.size() - 1].size() << ") does not equal the number of columns in the header (" << this->table[0].size() << ")\n";
					this->table.pop_back();
					return;
				}
			}
			f.close();
		}
		else
		{
			std::cout << "ERROR: (While trying to open " << file << ") Cannot open file\n";
		}
	}
	else
	{
		std::cout << "ERROR: (While trying to open " << file << ") File extension not supported\n";
	}
}

db::table::Table::Table(std::vector<std::string> cols)
{
	this->sort_rule = {true, 0};
	this->table.push_back(cols);
}

db::table::SubTable::SubTable(Table* target)
{
	this->target = target;
}

size_t db::table::Table::get_col_index(std::string col_name)
{
	for(int i = 0; i < this->table[0].size(); i++)
		if(this->table[0][i] == col_name)
			return i;
	return -1;
}

void db::table::Table::print()
{
	int cell_width = 120 / (this->table[0].size() + 1);

	std::string bar;
	for(int i = 0; i < cell_width; i++)
		bar += "─";

	std::cout << "┌─"; 
	for(int i = 0; i < this->table[0].size(); i++)
		std::cout << bar << "─┬─";
	std::cout << bar << "─┐\n";

	for(int i = 0; i < this->table.size(); i++)
	{
		std::cout << "│ ";

		std::string value = std::to_string(i);
			
		if(value.length() <= cell_width)
		{
			std::cout << value;
			for(int k = 0; k < (cell_width - value.length()); k++)
				std::cout << " ";
		}
		else
		{
			std::cout << value.substr(0, cell_width - 3) << "...";
		}

		std::cout << " │ ";

		for(int j = 0; j < this->table[0].size(); j++)
		{
			std::string& value = this->table[i][j];
			
			if(value.length() <= cell_width)
			{
				std::cout << value;
				for(int k = 0; k < (cell_width - value.length()); k++)
					std::cout << " ";
			}
			else
			{
				std::cout << value.substr(0, cell_width - 3) << "...";
			}

			std::cout << " │ ";
		}

		if(i < this->table.size() - 1)
		{
			std::cout << "\n├─"; 
			for(int i = 0; i < this->table[0].size(); i++)
				std::cout << bar << "─┼─";
			std::cout << bar << "─┤";
		}
		std::cout << "\n";
	}
	
	std::cout << "└─"; 
	for(int i = 0; i < this->table[0].size(); i++)
		std::cout << bar << "─┴─";
	std::cout << bar << "─┘\n";
}

void db::table::SubTable::print()
{
	int cell_width = 120 / (this->target->table[0].size() + 1);

	std::string bar;
	for(int i = 0; i < cell_width; i++)
		bar += "─";

	std::cout << "┌─"; 
	for(int i = 0; i < this->target->table[0].size(); i++)
		std::cout << bar << "─┬─";
	std::cout << bar << "─┐\n";

	for(int i = 0; i < this->rows.size(); i++)
	{
		std::cout << "│ ";

		std::string value = std::to_string(this->rows[i]);
			
		if(value.length() <= cell_width)
		{
			std::cout << value;
			for(int k = 0; k < (cell_width - value.length()); k++)
				std::cout << " ";
		}
		else
		{
			std::cout << value.substr(0, cell_width - 3) << "...";
		}

		std::cout << " │ ";

		for(int j = 0; j < this->target->table[0].size(); j++)
		{
			std::string& value = this->target->table[this->rows[i]][j];
			
			if(value.length() <= cell_width)
			{
				std::cout << value;
				for(int k = 0; k < (cell_width - value.length()); k++)
					std::cout << " ";
			}
			else
			{
				std::cout << value.substr(0, cell_width - 3) << "...";
			}

			std::cout << " │ ";
		}

		if(i < this->rows.size() - 1)
		{
			std::cout << "\n├─"; 
			for(int i = 0; i < this->target->table[0].size(); i++)
				std::cout << bar << "─┼─";
			std::cout << bar << "─┤";
		}
		std::cout << "\n";
	}
	
	std::cout << "└─"; 
	for(int i = 0; i < this->target->table[0].size(); i++)
		std::cout << bar << "─┴─";
	std::cout << bar << "─┘\n";
}

void db::table::Table::save(std::string path)
{
	std::ofstream out(path);

	for(int i = 0; i < this->table.size(); i++)
	{
		for(int j = 0; j < this->table[0].size() - 1; j++)
		{
			out << this->table[i][j] << ",";
		}
		out << this->table[i][this->table[0].size() - 1] << "\n";
	}
}

void db::table::SubTable::save(std::string path)
{
	std::cout << "INFO: Saving as " << path << "...\n";
}

void db::table::Table::set(std::string col, std::string value)
{
	// TODO this is ass
	int col_index = 0;
	for(int i = 0; i < this->table[0].size(); i++)
	{
		if(this->table[0][i] == col)
		{
			col_index = i;
			break;
		}
	}

	for(int i = 1; i < this->table.size(); i++)
		this->table[i][col_index] = value;
}

void db::table::Table::sort()
{
	auto first = this->table[0];
	this->table.erase(this->table.begin());
	this->table = db::utils::quicksort_table(this->table, this->sort_rule.column_index, this->sort_rule.ascending);
	this->table.insert(this->table.begin(), first);
}

void db::table::SubTable::sort()
{
	std::cout << "TODO: SubTable::sort()\n";
}

void db::table::Table::insert(std::vector<std::string> row)
{
	// Edge cases

    if(this->table.size() <= 1)
    {
        this->table.push_back(row);
        return;
    }

	if(this->sort_rule.ascending && db::utils::is_greater(row[this->sort_rule.column_index], this->table[this->table.size() - 1][this->sort_rule.column_index]))
	{
		this->table.push_back(row);
        return;
	} 

	// Binary search

	int low = 1;
	int high = this->table.size() - 1;
	
	if(this->sort_rule.ascending)
	{
		while(low < high)
		{
			int mid = ((high - low) / 2) + low;

			if(db::utils::is_greater(this->table[mid][this->sort_rule.column_index], row[this->sort_rule.column_index]))
            	high = mid - 1;
			else
				low = mid + 1;
		}
	}
	else
	{
		while(low < high)
		{
			int mid = ((high - low) / 2) + low;

			if(db::utils::is_greater(row[this->sort_rule.column_index], this->table[mid][this->sort_rule.column_index]))
            	high = mid - 1;
			else
				low = mid + 1;
		}
	}

	// Insert
	this->table.insert(this->table.begin() + low, row);

	// Weird edge case
	if(
	low < (this->table.size() - 1)
	&& this->sort_rule.ascending
	&& db::utils::is_greater(this->table[low][this->sort_rule.column_index], this->table[low + 1][this->sort_rule.column_index]))
	{
		std::swap(this->table[low], this->table[low + 1]);
	}
	else if(
	low < (this->table.size() - 1)
	&& !this->sort_rule.ascending
	&& db::utils::is_greater(this->table[low + 1][this->sort_rule.column_index], this->table[low][this->sort_rule.column_index]))
	{
		std::swap(this->table[low], this->table[low + 1]);
	}
}

/*
5
1, 8
   v           v           v
1, 2, 3, 4, 5, 6, 6, 7, 8, 9

*/