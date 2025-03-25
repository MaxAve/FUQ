#include <interpreter.hpp>

void db::interpreter::run(std::string line)
{
	auto test1 = db::parser::normalize(line, ' ');
	std::cout << test1 << "\n";
	auto test2 = db::parser::split(test1, ' ');
	for(auto& s : test2)
		std::cout << s << ", ";
	std::cout << "\n";
}
