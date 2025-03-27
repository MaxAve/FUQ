#include <interpreter.hpp>

db::interpreter::AST::AST(std::vector<token_t>& tokens)
{
	if(db::parser::get(tokens, 1) == "(")
	{
		/*
		Function call
		*/

		std::vector<token_t> toks;

		int scope_level = 0;

		for(int i = 1; i < tokens.size(); i++)
		{
			if(tokens[i] == "(")
				scope_level++;
			else if(tokens[i] == ")")
				scope_level--;

			if((tokens[i] == "," && scope_level == 1) || (tokens[i] == ")" && scope_level == 0))
			{
				this->children.push_back(db::interpreter::AST(toks));
				toks.clear();
				continue;
			}

			toks.push_back(tokens[i]);
		}
	}
	else
	{
		/*
		Expression
		*/
	}
}

void db::interpreter::run(std::string line)
{
	line = db::parser::normalize(line, ' ', true);
	std::vector<token_t> tokens = db::parser::tokenize(line);
	db::interpreter::AST ast(tokens);
}
