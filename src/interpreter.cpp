#include <interpreter.hpp>

void db::interpreter::run(std::string line)
{
	line = db::parser::normalize(line, ' ', true);
	std::vector<std::string> tokens = db::parser::tokenize(line);
	for(int i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << "\n";
}
