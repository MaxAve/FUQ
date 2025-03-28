#include <interpreter.hpp>

db::interpreter::AST::AST(std::string val)
{
	this->value = val;
	this->type = db::script::TokenType::VALUE;
}

db::interpreter::AST::AST(const std::vector<token_t>& tokens)
{
	if(db::parser::get(tokens, 1) == "(")
	{
		/*
		Function call
		*/

		this->type = db::script::FUNCTION_CALL;
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

		this->type = db::script::EXPRESSION;

		// Find operator with highest precedence
		uint8_t highest_precedence = 0;
		int highest_precedence_index = -1;
		for(int i =  0; i < tokens.size(); i++)
		{
			if(db::script::operators.find(tokens[i]) != db::script::operators.end()
			&& db::script::operator_info.at(db::script::operators.at(tokens[i])).precedence >= highest_precedence)
			{
				highest_precedence = db::script::operator_info.at(db::script::operators.at(tokens[i])).precedence;
				highest_precedence_index = i;
			}
		}

		// Split expression
		if(highest_precedence_index >= 0)
		{
			this->children.push_back(AST(tokens[highest_precedence_index]));

			std::vector<token_t> left;
			std::vector<token_t> right;

			for(int i = 0; i < highest_precedence_index; i++)
				left.push_back(tokens[i]);

			for(int i = highest_precedence_index + 1; i < tokens.size(); i++)
				right.push_back(tokens[i]);

			this->children.push_back(AST(left));
			this->children.push_back(AST(right));
		}
	}
	else
	{
		/*
		Value
		*/
		this->type = db::script::TokenType::VALUE;
		for(int i = 0; i < tokens.size(); i++)
			this->value += tokens[i];
	}
}

void db::interpreter::AST::print(int depth)
{
	for(int i = 0; i < depth; i++)
		std::cout << "    ";
	switch(this->type)
	{
		case db::script::TokenType::VALUE:
			std::cout << "[VALUE] ";
			break;
		case db::script::TokenType::FUNCTION_CALL:
			std::cout << "[FUNCTION_CALL] ";
			break;
		case db::script::TokenType::EXPRESSION:
			std::cout << "[EXPRESSION] ";
			break;
	}
	std::cout << this->value << "\n";
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

void db::interpreter::InterpreterContex::run_function(const db::interpreter::AST &fcall)
{
	if(db::script::function_infos.find(fcall.value) == db::script::function_infos.end())
	{
		std::cout << "ERROR: Function '" << fcall.value << "' does not exist.\n";
		return;
	}
	switch(db::script::function_infos.at(fcall.value).id)
	{
		case db::script::FunctionID::FILTER:
		{
			std::cout << "call filter\n";
			break;
		}
		case db::script::FunctionID::SET:
		{
			std::cout << "call set\n";
			break;
		}
	}
}

void db::interpreter::InterpreterContex::run(std::string line)
{
	db::interpreter::AST ast(db::parser::tokenize(db::parser::normalize(line, ' ', true)));

	switch(ast.type)
	{
		case db::script::FUNCTION_CALL:
		{
			this->run_function(ast);
			break;
		}
	}
}
