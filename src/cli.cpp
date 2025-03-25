#include <cli.hpp>

void db::cli::run()
{
	while(1)
	{
		std::string input;
		std::cout << ">>> ";
		std::getline(std::cin, input);

		if(input == "exit")
			break;

		db::interpreter::run(input);
	}
}
