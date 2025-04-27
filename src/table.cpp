#include <table.hpp>

db::table::Table::Table(std::string file)
{
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
	std::cout << "INFO: Saving as " << path << "...\n";
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

void db::table::SubTable::set(std::string col, std::string value)
{
	// TODO this is ass
	int col_index = 0;
	for(int i = 0; i < this->target->table[0].size(); i++)
	{
		if(this->target->table[0][i] == col)
		{
			col_index = i;
			break;
		}
	}

	for(int i = 1; i < this->rows.size(); i++)
		this->target->table[this->rows[i]][col_index] = value;
}