#include <table.hpp>

db::table::Table::Table(std::string file)
{
	this->sort_rule = {true, 0};

	if(file.substr(file.length() - 4, 4) == ".csv")
	{
		std::ifstream f(file);

		std::string line;

		if(f.is_open())
		{
			while(std::getline(f, line))
			{
				this->table.push_back(std::vector<std::string>());
				this->table[this->table.size() - 1].push_back("");
				for(int i = 0; i < line.length(); i++)
				{
					if(line[i] == ',')
					{
						this->table[this->table.size() - 1].push_back("");
						continue;
					}
					this->table[this->table.size() - 1][this->table[this->table.size() - 1].size() - 1] += line[i];
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
	// TODO this is slower that your GRANDMOTHER
	bool sorted = false;
	while(!sorted)
	{
		sorted = true;
		for(int i = 1; i < this->table.size() - 1; i++)
		{
			if(this->sort_rule.ascending)
			{
				if(db::utils::is_greater(this->table[i][this->sort_rule.column_index], this->table[i + 1][this->sort_rule.column_index]))
				{
					auto tmp = this->table[i];
					this->table[i] = this->table[i + 1];
					this->table[i + 1] = tmp;
					sorted = false;
				}
			}
			else
			{
				if(db::utils::is_greater(this->table[i + 1][this->sort_rule.column_index], this->table[i][this->sort_rule.column_index]))
				{
					auto tmp = this->table[i];
					this->table[i] = this->table[i + 1];
					this->table[i + 1] = tmp;
					sorted = false;
				}
			}
		}
	}
}

void db::table::SubTable::sort()
{
	std::cout << "TODO: SubTable::sort()\n";
}