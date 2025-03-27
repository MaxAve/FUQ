#include <interpreter.hpp>

db::interpreter::AST::AST(const std::vector<token_t>& tokens)
{
	if(db::parser::get(tokens, 1) == "(")
	{
		/*
		Function call
		*/

		this->type = FUNCTION_CALL;
		this->value = tokens[0];

		std::vector<token_t> toks;

		int scope_level = 1;

		for(int i = 2; i < tokens.size(); i++)
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
	else if(db::interpreter::is_expression(tokens))
	{
		/*
		Expression
		*/

		this->type = EXPRESSION;

		for(int i =  0; i < tokens.size(); i++)
		{
			this->children.push_back(db::interpreter::AST({tokens[i]}));
		}
	}
	else
	{
		/*
		Value
		*/
		this->type = VALUE;
		for(int i = 0; i < tokens.size(); i++)
			this->value += tokens[i];
	}
}

void db::interpreter::AST::print(int depth)
{
	for(int i = 0; i < depth; i++)
		std::cout << "    ";
	std::cout << this->value << ": ";
	switch(this->type)
	{
		case VALUE:
			std::cout << "(VALUE)";
			break;
		case FUNCTION_CALL:
			std::cout << "(FUNCTION_CALL)";
			break;
		case EXPRESSION:
			std::cout << "(EXPRESSION)";
			break;
	}
	std::cout << "\n";
	for(auto& child : this->children)
	{
		child.print(depth + 1);
	}
}

bool db::interpreter::is_expression(const std::vector<token_t>& tokens)
{
	if(tokens.size() == 1)
		return false;
	for(int i = 0; i < tokens.size(); i++)
		if(db::script::operators.find(tokens[i]) != db::script::operators.end())
			return true;
	return false;
}

void db::interpreter::run(std::string line)
{
	line = db::parser::normalize(line, ' ', true);
	std::vector<token_t> tokens = db::parser::tokenize(line);
	db::interpreter::AST ast(tokens);
	ast.print();
}
