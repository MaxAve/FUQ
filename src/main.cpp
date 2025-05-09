#include <iostream>
#include <fstream>
#include <string>
#include <cli.hpp>
#include <interpreter.hpp>

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		db::cli::run();
	}
	else
	{
		int line_number = 1;
		std::string path(argv[1]);
		db::interpreter::Context ctx;
		std::ifstream file(path);
		std::string line;
		if (file.is_open()) {
			while(getline(file, line))
			{
				ctx.run(line);
				if(ctx.err)
				{
					std::cout << "   " << line_number << " |      " << line << "\n";
					return 0;
				}
				line_number++;
			}
			file.close();
		}
		else 
		{
			std::cerr << "Could not open " << path << "\n";
		}
	}
    return 0;
}
