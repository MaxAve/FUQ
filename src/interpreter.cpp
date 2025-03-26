#include <interpreter.hpp>

void db::interpreter::run(std::string line)
{
	std::vector<std::string> tokens = db::parser::tokenize("set(filter(table, number == 69), number, 'nice')");
	std::cout << "[";
	for(int i = 0; i < tokens.size() - 1; i++)
		std::cout << "'" << tokens[i] << "', ";
	std::cout << "'" << tokens[tokens.size() - 1] << "']\n";
	for(int i = 0; i < tokens.size(); i++)
		std::cout << tokens[i].length() << "\n";
}
