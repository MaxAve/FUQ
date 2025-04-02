#include <cli.hpp>

void db::cli::run()
{
	db::interpreter::Context ctx;

	while(1)
	{
		std::string input;
		std::cout << ">>> ";
		std::getline(std::cin, input);

		if(input == "exit")
			break;

		ctx.run(input);
	}
}
