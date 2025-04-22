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
	else if(db::interpreter::is_expression(tokens) && !db::interpreter::is_string(tokens))
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

bool db::interpreter::is_string(const std::vector<token_t>& tokens)
{
	return tokens[0][0] == '"' || tokens[0][0] == '\'';
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

void db::interpreter::Context::load_table(std::string file, std::string newname)
{
	// TODO subtables
	if(this->tables.find(newname) != this->tables.end())
		delete this->tables[newname];
	auto t = new db::table::Table(file);
	if(t->table.size() > 0)
		this->tables[newname] = t;
}

void db::interpreter::Context::unload_table(std::string name)
{
	// TODO subtables
	if(this->tables.find(name) != this->tables.end())
	{
		delete this->tables[name];
		this->tables.erase(name);
	}
	else
	{
		std::cout << "ERROR: (While trying to delete " << name << ") No table or sub-table loaded\n";
	}
}

std::string db::interpreter::Context::call_function(const db::interpreter::AST &fcall)
{
	if(db::script::function_infos.find(fcall.value) == db::script::function_infos.end())
	{
		std::cout << "ERROR: Function '" << fcall.value << "' does not exist. Type 'flist' for a list of function definitions.\n";
		return "";
	}

	db::interpreter::FCall fc;
	fc.fid = db::script::function_infos.at(fcall.value).id;
	fc.ret = this->call_stack.size(); // TODO
	fc.stack_index = this->call_stack.size();

	// Check for the right amount of parameters and print an error message if the parameters don't match with what is expected
	if(db::script::function_infos.at(fcall.value).expect_parameters >= 0 && fcall.children.size() != db::script::function_infos.at(fcall.value).expect_parameters)
	{
		std::cout << "ERROR: In function '" << fcall.value << "': expected " << (int)db::script::function_infos.at(fcall.value).expect_parameters << " parameters but got " << fcall.children.size() << ".\n";
		return "";
	}

	// We have to push the fcall since at it may call other functions before evaluating all parameters
	this->call_stack.push_back(fc);

	// Evaluate parameters (values are added directly, expressions and functions are first evaluated)
	for(int i = 0; i < fcall.children.size(); i++)
	{
		// TODO what to do with conditions/expressions?
		switch(fcall.children[i].type)
		{
			case db::script::TokenType::VALUE:
			{
				fc.params.push_back(fcall.children[i].value);
				break;
			}
			case db::script::TokenType::FUNCTION_CALL:
			{
				fc.params.push_back(db::interpreter::Context::call_function(fcall.children[i]));
				break;
			}
			case db::script::TokenType::EXPRESSION:
			{
				fc.params.push_back("EXPRESSION");
				break;
			}
		}
	}

	for(int i = 0; i < fc.params.size(); i++)
	{
		// Trim away matching quotation marks
		if((fc.params[i][0] == '"' || fc.params[i][0] == '\'') && fc.params[i][0] == fc.params[i][fc.params[i].length() - 1])
		{
			fc.params[i] = fc.params[i].substr(1, fc.params[i].length() - 2);
		}
	}
	
	std::cout << "Params: ";
	for(int i = 0; i < fc.params.size(); i++)
		std::cout << "[" << fc.params[i] << "] ";
	std::cout << "\n";
	
	// Certain functions will return a sub-table, which acts like an independant table with a specifc name.
	// If a table is returned, ret is to be set to the name of the new table so that it can be used by higher
	// function calls.
	std::string ret = "";

	// Corresponding function calls
	switch(fc.fid)
	{
		case db::script::FunctionID::PRINT:
		{
			std::cout << "call: print\n";
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to print " << fc.params[0] << ") Table or sub-table is not loaded\n";
				}
				else
				{
					this->subtables[fc.params[0]]->print();
				}
			}
			else
			{
				this->tables[fc.params[0]]->print();
			}
			break;
		}
		case db::script::FunctionID::PRINTS:
		{
			std::cout << fc.params[0] << "\n";
			break;
		}
		case db::script::FunctionID::LOAD:
		{
			std::cout << "call: load\n";
			this->load_table(fc.params[0], fc.params[1]);
			break;
		}
		case db::script::FunctionID::UNLOAD:
		{
			std::cout << "call: unload\n";
			this->unload_table(fc.params[0]);
			break;
		}
		case db::script::FunctionID::SAVE:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to print " << fc.params[0] << ") Table or sub-table is not loaded\n";
				}
				else
				{
					this->subtables[fc.params[0]]->save(fc.params[1]);
				}
			}
			else
			{
				this->tables[fc.params[0]]->save(fc.params[1]);
			}
			break;
		}
		case db::script::FunctionID::FILTER:
		{
			std::cout << "call: filter\n";

			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				std::cout << "ERROR: (While trying to filter " << fc.params[0] << ") No table loaded\n";
				break;
			}

			std::string subtable_name = std::string("__fnret") + std::to_string(fc.stack_index) + "__";

			db::table::SubTable* st = new db::table::SubTable(this->tables[fc.params[0]]);
			
			for(size_t i = 0; i < this->tables[fc.params[0]]->table.size(); i++)
			{
				// TODO this is a placeholder condition
				if(i % 2 == 0)
				{
					st->rows.push_back(i);
					std::cout << "ROW: " << i << "\n";
				}
			}

			this->subtables[subtable_name] = st;

			ret = subtable_name;

			break;
		}
		case db::script::FunctionID::SET:
		{
			std::cout << "call: set\n";
			break;
		}
		case db::script::FunctionID::ERASE:
		{
			std::cout << "call: erase\n";
			break;
		}
		case db::script::FunctionID::INSERT:
		{
			std::cout << "call: insert\n";
			break;
		}
	}

	int calls_to_pop = this->call_stack.size() - fc.stack_index;
	for(int i = 0; i < calls_to_pop; i++)
		this->call_stack.pop_back();

	return ret;
}

void db::interpreter::Context::run(std::string line)
{
	if(db::parser::filter(line, ' ') == "flist")
		std::cout << "print(name: value) -> none\nprints(string: value) -> none\nload(file: value, name: value) -> none\nunload(name: value) -> none\nsave(name: value, path: value) -> none\nset(table: ptr_list, row: value, val: value/expression) -> none\nfilter(table: ptr_list, condition: value/expression) -> ptr_list\ninsert(table: value, row: value/expression...) -> none\nerase(table: ptr_list) -> none\n\nFor detailed explanations for usage, type 'fhelp <function>' with <function> as the corresponding function name.\n";

	std::string normalized = db::parser::normalize(line, ' ', true);
	//std::cout << "=== NORMALIZED ===\n" << normalized << "\n";
	
	std::vector<std::string> tokens = db::parser::tokenize(normalized);
	//std::cout << "=== TOKENIZER ===\n[";
	//for(int i = 0; i < tokens.size() - 1; i++)
	//	std::cout << "'" << tokens[i] << "', ";
	//std::cout << "'" << tokens[tokens.size() - 1] << "']\n";

	db::interpreter::AST ast(tokens);
	//std::cout << "=== AST ===\n";
	//ast.print();

	switch(ast.type)
	{
		case db::script::FUNCTION_CALL:
		{
			this->call_function(ast);
			break;
		}
	}
}
