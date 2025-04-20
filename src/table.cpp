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

void db::table::Table::print()
{
	int cell_width = 120 / this->table[0].size();

	std::string bar;
	for(int i = 0; i < cell_width; i++)
		bar += "─";

	std::cout << "┌─"; 
	for(int i = 0; i < this->table[0].size() - 1; i++)
		std::cout << bar << "─┬─";
	std::cout << bar << "─┐\n";

	for(int i = 0; i < this->table.size(); i++)
	{
		std::cout << "│ ";
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
			for(int i = 0; i < this->table[0].size() - 1; i++)
				std::cout << bar << "─┼─";
			std::cout << bar << "─┤";
		}
		std::cout << "\n";
	}
	
	std::cout << "└─"; 
	for(int i = 0; i < this->table[0].size() - 1; i++)
		std::cout << bar << "─┴─";
	std::cout << bar << "─┘\n";
}
