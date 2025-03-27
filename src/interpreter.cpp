#include <interpreter.hpp>

void db::interpreter::run(std::string line)
{
	std::vector<std::string> tokens = db::parser::tokenize("set(filter(table, number == 69 or number == 420), number, 'funi number')");
	for(int i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << "\n";
}
