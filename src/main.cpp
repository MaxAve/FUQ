#include <iostream>
#include <fstream>
#include <string>

#include <table.hpp>
#include <script.hpp>
#include <parser.hpp>
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
		std::string path(argv[1]);
		db::interpreter::Context ctx;
		std::ifstream file(path);
		std::string line;
		if (file.is_open()) {
			while(getline(file, line))
			{
				ctx.run(line);
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
