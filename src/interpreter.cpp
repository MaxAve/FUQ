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

db::interpreter::Context::~Context()
{
	std::vector<std::string> todelete;
	for(auto& it : this->tables)
	{
		todelete.push_back(it.first);
	}
	for(int i = 0; i < todelete.size(); i++)
	{
		this->unload_table(todelete[i]);
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
		this->err = 1;
	}
}

std::string db::interpreter::Context::call_function(const db::interpreter::AST &fcall)
{
	if(db::script::function_infos.find(fcall.value) == db::script::function_infos.end())
	{
		std::cout << "ERROR: Function '" << fcall.value << "' does not exist. Type 'flist' for a list of functions.\n";
		this->err = 1;
		return "";
	}

	db::interpreter::FCall fc = {db::script::function_infos.at(fcall.value).id, {}, this->call_stack.size(), this->call_stack.size()};
	// fc.fid = db::script::function_infos.at(fcall.value).id;
	// fc.ret = this->call_stack.size(); // TODO
	// fc.stack_index = this->call_stack.size();

	// Check for the right amount of parameters and print an error message if the parameters don't match with what is expected
	if(db::script::function_infos.at(fcall.value).expect_parameters >= 0 && fcall.children.size() != db::script::function_infos.at(fcall.value).expect_parameters)
	{
		std::cout << "ERROR: In function '" << fcall.value << "': expected " << (int)db::script::function_infos.at(fcall.value).expect_parameters << " parameters but got " << fcall.children.size() << ".\n";
		this->err = 1;
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
				// Edge case where negative numbers are parsed as expressions
				if(fcall.children[i].children[0].value == "-" && fcall.children[i].children[1].value.length() == 0 && db::utils::is_number(fcall.children[i].children[2].value))
				{
					fc.params.push_back(fcall.children[i].children[0].value + fcall.children[i].children[2].value);
				}

				fc.params.push_back("__fuq_lambda");
				this->lambdas.push_back({{}, fcall.children[i]});
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
	
	// std::cout << "Params: ";
	// for(int i = 0; i < fc.params.size(); i++)
	// 	std::cout << "[" << fc.params[i] << "] ";
	// std::cout << "\n";
	
	// Certain functions will return a sub-table, which acts like an independant table with a specifc name.
	// If a table is returned, ret is to be set to the name of the new table so that it can be used by higher
	// function calls.
	std::string ret = "";

	// Corresponding function calls
	switch(fc.fid)
	{
		case db::script::FunctionID::PRINTT:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to print " << fc.params[0] << ") Table or sub-table is not loaded\n";
					this->err = 1;
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
			this->load_table(fc.params[0], fc.params[1]);
			break;
		}
		case db::script::FunctionID::CREATE:
		{
			if(fc.params.size() < 2)
			{
				std::cout << "ERROR: create expects at least 2 parameters but got " << fc.params.size() << "\n";
				break;
			}
			if(this->tables.find(fc.params[0]) != this->tables.end())
			{
				std::cout << "ERROR: (While trying to create " << fc.params[0] << ") Table already exists\n";
				break;
			}
			std::string tn = fc.params[0];
			fc.params.erase(fc.params.begin());
			this->tables[tn] = new db::table::Table(fc.params);
			break;
		}
		case db::script::FunctionID::UNLOAD:
		{
			this->unload_table(fc.params[0]);
			break;
		}
		case db::script::FunctionID::SAVE:
		{
			if(this->tables.find(fc.params[1]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[1]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to save " << fc.params[1] << ") Table or sub-table is not loaded\n";
					this->err = 1;
				}
				else
				{
					this->subtables[fc.params[1]]->save(fc.params[0]);
				}
			}
			else
			{
				this->tables[fc.params[1]]->save(fc.params[0]);
			}
			break;
		}
		case db::script::FunctionID::FILTER:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to filter " << fc.params[0] << ") Table or sub-table is not loaded\n";
					this->err = 1;
				}
				else
				{
					std::string subtable_name = std::string("__fuq_fnret") + std::to_string(fc.stack_index);

					db::table::SubTable* st = new db::table::SubTable(this->subtables[fc.params[0]]->target);
					
					for(size_t i = 0; i < this->subtables[fc.params[0]]->rows.size(); i++)
					{
						// Row values as lambda params
						for(int j = 0; j < this->subtables[fc.params[0]]->target->table[0].size(); j++)
						{
							this->lambdas[this->lambdas.size() - 1].params[this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->table[this->subtables[fc.params[0]]->rows[i]][j];
						}

						this->lambdas[this->lambdas.size() - 1].params["INDEX"] = std::to_string(this->subtables[fc.params[0]]->rows[i]);
						
						/*for(int j = 0; j < this->subtables[fc.params[0]]->target->table[0].size(); j++)
						{
							this->lambdas[this->lambdas.size() - 1].params[std::string("MAX_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->max_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
							this->lambdas[this->lambdas.size() - 1].params[std::string("MIN_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->min_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
							this->lambdas[this->lambdas.size() - 1].params[std::string("AVG_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->avg_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
						}*/

						if(db::interpreter::Context::evaluate_lambda(this->lambdas[this->lambdas.size() - 1]) == "1")
						{
							st->rows.push_back(this->subtables[fc.params[0]]->rows[i]);
						}
					}

					this->lambdas.pop_back();

					this->subtables[subtable_name] = st;

					ret = subtable_name;
				}
			}
			else
			{
				std::string subtable_name = std::string("__fuq_fnret") + std::to_string(fc.stack_index);
				
				db::table::SubTable* st = new db::table::SubTable(this->tables[fc.params[0]]);
				
				for(size_t i = 1; i < this->tables[fc.params[0]]->table.size(); i++)
				{
					// Row values as lambda params
					for(int j = 0; j < this->tables[fc.params[0]]->table[0].size(); j++)
					{
						this->lambdas[this->lambdas.size() - 1].params[this->tables[fc.params[0]]->table[0][j]] = this->tables[fc.params[0]]->table[i][j];
					}

					this->lambdas[this->lambdas.size() - 1].params["INDEX"] = std::to_string(i);

					if(db::interpreter::Context::evaluate_lambda(this->lambdas[this->lambdas.size() - 1]) == "1")
					{
						st->rows.push_back(i);
					}
				}

				this->lambdas.pop_back();

				this->subtables[subtable_name] = st;

				ret = subtable_name;
			}
			break;
		}
		case db::script::FunctionID::SET:
		{
			if(fc.params[2] == "__fuq_lambda")
			{
				if(this->tables.find(fc.params[0]) == this->tables.end())
				{
					if(this->subtables.find(fc.params[0]) == this->subtables.end())
					{
						std::cout << "ERROR: (While trying to set " << fc.params[0] << ") Table or sub-table is not loaded\n";
						this->err = 1;
					}
					else
					{
						int col_index = this->subtables[fc.params[0]]->target->get_col_index(fc.params[1]);

						for(int i = 0; i < this->subtables[fc.params[0]]->rows.size(); i++)
						{
							for(int j = 0; j < this->subtables[fc.params[0]]->target->table[0].size(); j++)
							{
								this->lambdas[this->lambdas.size() - 1].params[this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->table[this->subtables[fc.params[0]]->rows[i]][j];
							}

							this->lambdas[this->lambdas.size() - 1].params["INDEX"] = std::to_string(i);
							/*for(int j = 0; j < this->subtables[fc.params[0]]->target->table[0].size(); j++)
							{
								this->lambdas[this->lambdas.size() - 1].params[std::string("MAX_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->max_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
								this->lambdas[this->lambdas.size() - 1].params[std::string("MIN_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->min_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
								this->lambdas[this->lambdas.size() - 1].params[std::string("AVG_") + this->subtables[fc.params[0]]->target->table[0][j]] = this->subtables[fc.params[0]]->target->avg_val[this->subtables[fc.params[0]]->target->table[0][j]]; 
							}*/

							this->subtables[fc.params[0]]->target->table[this->subtables[fc.params[0]]->rows[i]][col_index] = db::interpreter::Context::evaluate_lambda(this->lambdas[this->lambdas.size() - 1]);
						}

						this->lambdas.pop_back();
					}
				}
				else
				{
					int col_index = this->tables[fc.params[0]]->get_col_index(fc.params[1]);

					for(int i = 1; i < this->tables[fc.params[0]]->table.size(); i++)
					{
						for(int j = 0; j < this->tables[fc.params[0]]->table[0].size(); j++)
						{
							this->lambdas[this->lambdas.size() - 1].params[this->tables[fc.params[0]]->table[0][j]] = this->tables[fc.params[0]]->table[i][j];
						}
						this->lambdas[this->lambdas.size() - 1].params["INDEX"] = std::to_string(i);
						this->tables[fc.params[0]]->table[i][col_index] = db::interpreter::Context::evaluate_lambda(this->lambdas[this->lambdas.size() - 1]);
					}

					this->lambdas.pop_back();
				}
			}
			else
			{
				if(this->tables.find(fc.params[0]) == this->tables.end())
				{
					if(this->subtables.find(fc.params[0]) == this->subtables.end())
					{
						std::cout << "ERROR: (While trying to set " << fc.params[0] << ") Table or sub-table is not loaded\n";
						this->err = 1;
					}
					else
					{
						size_t col_index = this->subtables[fc.params[0]]->target->get_col_index(fc.params[1]);
						for(int i = 0; i < this->subtables[fc.params[0]]->rows.size(); i++)
						{
							if(fc.params[2] == "[INDEX]")
								this->subtables[fc.params[0]]->target->table[this->subtables[fc.params[0]]->rows[i]][col_index] = std::to_string(i); // This is an edge case for something like set(table, [INDEX])
							else
								this->subtables[fc.params[0]]->target->table[this->subtables[fc.params[0]]->rows[i]][col_index] = fc.params[2];
						}
					}
				}
				else
				{
					size_t col_index = this->tables[fc.params[0]]->get_col_index(fc.params[1]);
					for(int i = 1; i < this->tables[fc.params[0]]->table.size(); i++)
					{
						if(fc.params[2] == "[INDEX]")
							this->tables[fc.params[0]]->table[i][col_index] = std::to_string(i); // This is an edge case for something like set(table, [INDEX]), since without any operators, [INDEX] would normally be parsed as a string
						else
							this->tables[fc.params[0]]->table[i][col_index] = fc.params[2];
					}
				}
			}
			break;
		}
		case db::script::FunctionID::ERASE:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to print " << fc.params[0] << ") Could not retrieve sub-table\n";
					this->err = 1;
				}
				else
				{
					// TODO this implementation is slower than your mother
					std::vector<std::vector<std::string>> table;
					for(int i = 0; i < this->subtables[fc.params[0]]->target->table.size(); i++)
					{
						bool contains = false;
						for(int j = 0; j < this->subtables[fc.params[0]]->rows.size(); j++)
						{
							if(i == this->subtables[fc.params[0]]->rows[j])
							{
								contains = true;
								break;
							}
						}
						if(contains)
							continue;
						table.push_back(this->subtables[fc.params[0]]->target->table[i]);
					}
					this->subtables[fc.params[0]]->target->table = table;
				}
			}
			else
			{
				this->tables[fc.params[0]]->table.clear();
			}
			break;
		}
		case db::script::FunctionID::INSERT:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				std::cout << "ERROR: (While trying to insert new row into " << fc.params[0] << ") No table loaded\n";
				break;
			}
			std::vector<std::string> new_row = fc.params;
			new_row.erase(new_row.begin());
			this->tables.at(fc.params[0])->insert(new_row);
			break;
		}
		case db::script::FunctionID::COLINSERT:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				std::cout << "ERROR: (While trying to insert new column into " << fc.params[0] << ") No table loaded\n";
				break;
			}
			this->tables.at(fc.params[0])->colinsert(fc.params[1]);
			break;
		}
		case db::script::FunctionID::COLERASE:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				std::cout << "ERROR: (While trying to erase column from " << fc.params[0] << ") No table loaded\n";
				break;
			}
			if(this->tables.at(fc.params[0])->get_col_index(fc.params[1]) == -1)
			{
				std::cout << "ERROR: (While trying to erase column from " << fc.params[0] << ") Column " << fc.params[1] << " does not exist\n";
				break;
			}
			this->tables.at(fc.params[0])->colerase(fc.params[1]);
			break;
		}
		case db::script::FunctionID::SORTRULE:
		{
			int index = this->tables.at(fc.params[0])->get_col_index(fc.params[1]);
			if(index == -1)
			{
				std::cout << "ERROR: (While trying to set sortrule for " << fc.params[0] << ") No column " << fc.params[1] << "\n";
				this->err = 1;
				break;
			}
			this->tables.at(fc.params[0])->sort_rule.column_index = index;
			this->tables.at(fc.params[0])->sort_rule.ascending = (fc.params[2] != "0");
			break;
		}
		case db::script::FunctionID::SORT:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				if(this->subtables.find(fc.params[0]) == this->subtables.end())
				{
					std::cout << "ERROR: (While trying to sort " << fc.params[0] << ") Table or sub-table is not loaded\n";
					this->err = 1;
				}
				else
				{
					this->subtables.at(fc.params[0])->sort();
				}
			}
			else
			{
				this->tables.at(fc.params[0])->sort();
			}
			break;
		}
		case db::script::FunctionID::JOIN:
		{
			if(this->tables.find(fc.params[0]) == this->tables.end())
			{
				std::cout << "ERROR: (While trying to join " << fc.params[0] << " and " << fc.params[1] << ") Table (" << fc.params[0] << ") not loaded\n";
				this->err = 1;
			}
			else
			{
				if(fc.params[2] != "0")
				{
					// Join and sort
					// TODO smarter join function
					if(this->tables.find(fc.params[1]) == this->tables.end())
					{
						if(this->subtables.find(fc.params[1]) == this->subtables.end())
						{
							std::cout << "ERROR: (While trying to join " << fc.params[0] << " and " << fc.params[1] << ") Table or sub-table is not loaded\n";
							this->err = 1;
						}
						else
						{
							for(int i = 0; i < this->subtables.at(fc.params[1])->rows.size(); i++)
							{
								this->tables.at(fc.params[0])->insert(this->subtables.at(fc.params[1])->target->table[this->subtables.at(fc.params[1])->rows[i]]);
							}
						}
					}
					else
					{
						const size_t s = this->tables.at(fc.params[1])->table.size(); // Prevents infinite loop
						for(int i = 1; i < s; i++)
						{
							this->tables.at(fc.params[0])->insert(this->tables.at(fc.params[1])->table[i]);
						}
					}
				}
				else
				{
					// Join without sorting
					if(this->tables.find(fc.params[1]) == this->tables.end())
					{
						if(this->subtables.find(fc.params[1]) == this->subtables.end())
						{
							std::cout << "ERROR: (While trying to join " << fc.params[0] << " and " << fc.params[1] << ") Table or sub-table is not loaded\n";
							this->err = 1;
						}
						else
						{
							for(int i = 0; i < this->subtables.at(fc.params[1])->rows.size(); i++)
							{
								this->tables.at(fc.params[0])->table.push_back(this->subtables.at(fc.params[1])->target->table[this->subtables.at(fc.params[1])->rows[i]]);
							}
						}
					}
					else
					{
						const size_t s = this->tables.at(fc.params[1])->table.size(); // Prevents infinite loop
						for(int i = 1; i < s; i++)
						{
							this->tables.at(fc.params[0])->table.push_back(this->tables.at(fc.params[1])->table[i]);
						}
					}
				}
			}
			break;
		}
		case db::script::FunctionID::HELP:
		{
			db::help::get_function_info(fc.params[0]);
		}
	}

	int calls_to_pop = this->call_stack.size() - fc.stack_index;
	for(int i = 0; i < calls_to_pop; i++)
		this->call_stack.pop_back();

	return ret;
}

std::string db::interpreter::Context::evaluate_lambda(db::interpreter::Lambda &lambda)
{
	// for(auto& it : lambda.params)
	// {
	// 	std::cout << it.first << " : \"" << it.second << "\"\n";
	// }

	for(int i = 0; i < lambda.code.children.size(); i++)
	{
		// Truncate spaces
		int end = lambda.code.children[i].value.size() - 1;
		for(int j = lambda.code.children[i].value.size() - 1; j >= 0; j--)
		{
			if(lambda.code.children[i].value[j] != ' ')
			{
				end = j;
				break;
			}
		}
		lambda.code.children[i].value = lambda.code.children[i].value.substr(0, end + 1);

		// Trim away matching quotation marks
		if((lambda.code.children[i].value[0] == '"' || lambda.code.children[i].value[0] == '\'') && lambda.code.children[i].value[0] == lambda.code.children[i].value[lambda.code.children[i].value.length() - 1])
		{
			lambda.code.children[i].value = lambda.code.children[i].value.substr(1, lambda.code.children[i].value.length() - 2);
		}
	}

	// NOTE: for single-operand operations (e.g. NOT), operand2 will be used; operand1 will remain uninitialized
	std::string operation=lambda.code.children[0].value, operand1, operand2;

	// 1st operand
	if(lambda.code.children[1].type == db::script::TokenType::EXPRESSION)
	{
		db::interpreter::Lambda l = {lambda.params, lambda.code.children[1]};
		operand1 = this->evaluate_lambda(l);
	}
	else
	{
		operand1 = lambda.code.children[1].value;
		if(operand1[0] == '[' && operand1[operand1.length() - 1] == ']')
		{
			std::string x = operand1.substr(1, operand1.length() - 2);
			if(lambda.params.find(x) != lambda.params.end())
				operand1 = lambda.params[x];
		}
	}
	
	// 2nd operand
	if(lambda.code.children[2].type == db::script::TokenType::EXPRESSION)
	{
		db::interpreter::Lambda l = {lambda.params, lambda.code.children[2]};
		operand2 = this->evaluate_lambda(l);
	}
	else
	{
		operand2 = lambda.code.children[2].value;
		if(operand2[0] == '[' && operand2[operand2.length() - 1] == ']')
		{
			std::string x = operand2.substr(1, operand2.length() - 2);
			if(lambda.params.find(x) != lambda.params.end())
				operand2 = lambda.params[x];
		}
	}	

	try
	{
		if(operation == "!")
		{
			if(operand2 != "0")
				return "0";
			return "1";
		}
		else if(operation == "&&")
		{
			if(operand1 != "0" && operand2 != "0")
				return "1";
			return "0";
		}
		else if(operation == "||")
		{
			if(operand1 != "0" || operand2 != "0")
				return "1";
			return "0";
		}
		else if(operation == "+")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((double)(std::stod(operand1) + std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) + std::stoll(operand2))); // int (long long)
			}
			else
			{
				return operand1 + operand2; // string (concat)
			}
		}
		else if(operation == "-")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((double)(std::stod(operand1) - std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) - std::stoll(operand2))); // int (long long)
			}
			else
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " - " << operand2 << ") Cannot substract two strings\n";
				this->err = 1;
				return "NULL";
			}
		}
		else if(operation == "*")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((double)(std::stod(operand1) * std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) * std::stoll(operand2))); // int (long long)
			}
			else
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " * " << operand2 << ") Cannot multiply two strings\n";
				this->err = 1;
				return "NULL";
			}
		}
		else if(operation == "/")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((double)(std::stod(operand1) / std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) / std::stoll(operand2))); // int (long long)
			}
			else
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " / " << operand2 << ") Cannot divide two strings\n";
				this->err = 1;
				return "NULL";
			}
		}
		else if(operation == "%")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
				{
					std::cout << "ERROR: (While trying to evaluate " << operand1 << " % " << operand2 << ") Cannot mod floating point numbers\n";
					this->err = 1;
				}
				else
					return std::to_string((long long)(std::stoll(operand1) % std::stoll(operand2))); // int (long long)
			}
			else
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " % " << operand2 << ") Cannot mod strings\n";
				this->err = 1;
				return "NULL";
			}
		}
		else if(operation == "^")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string(std::pow(std::stod(operand1), std::stod(operand2)));
				else
					return std::to_string((long long)(std::pow(std::stoll(operand1), std::stoll(operand2)))); // int (long long)
			}
			else
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " ^ " << operand2 << ") Cannot pow two strings\n";
				this->err = 1;
				return "NULL";
			}
		}
		else if(operation == "==")
		{
			return (operand1 == operand2) ? "1" : "0";
		}
		else if(operation == "!=")
		{
			return (operand1 == operand2) ? "1" : "0";
		}
		else if(operation == ">")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((float)(std::stod(operand1) > std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) > std::stoll(operand2))); // int (long long)
			}
			else
			{
				// TODO strings
				return "1";
			}
		}
		else if(operation == "<")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((float)(std::stod(operand1) < std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) < std::stoll(operand2))); // int (long long)
			}
			else
			{
				// TODO strings
				return "1";
			}
		}
		else if(operation == ">=")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((float)(std::stod(operand1) >= std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) >= std::stoll(operand2))); // int (long long)
			}
			else
			{
				// TODO strings
				return "1";
			}
		}
		else if(operation == "<=")
		{
			if(db::utils::is_number(operand1) && db::utils::is_number(operand2))
			{
				if(db::utils::is_float(operand1) || db::utils::is_float(operand2))
					return std::to_string((float)(std::stod(operand1) <= std::stod(operand2))); // double
				else
					return std::to_string((long long)(std::stoll(operand1) <= std::stoll(operand2))); // int (long long)
			}
			else
			{
				// TODO strings
				return "1";
			}
		}
		else if(operation == ".")
		{
			int index = std::stoi(operand2);
			if(index < 0 || index >= operand1.length())
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " . " << operand2 << ") Index out of range\n";
				return "";
			}
			return std::string(1, operand1[index]);
		}
		else if(operation == "$")
		{
			int index = std::stoi(operand2);
			if(index < 0 || index >= operand1.length())
			{
				std::cout << "ERROR: (While trying to evaluate " << operand1 << " -> " << operand2 << ") Index out of range\n";
				return "";
			}
			return operand1.substr(index, operand1.length() - index);
		}
		else if(operation == ":")
		{
			return operand1.substr(0, std::min(std::stoi(operand2), (int)operand1.length()));
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Encountered unexpected error while evaluating \"" << operand1 << "\" " << operation << " \"" << operand2 << "\"\n" << e.what() << "\n";
	}

	return "0";
}

void db::interpreter::Context::run(std::string line)
{
	std::string normalized = db::parser::normalize(line, ' ', true);

	std::vector<std::string> tokens = db::parser::tokenize(normalized);

	if(tokens.size() == 1 && tokens[0] == "flist")
	{
		db::help::list_all_functions();
	}

	db::interpreter::AST ast(tokens);

	//ast.print();

	switch(ast.type)
	{
		case db::script::FUNCTION_CALL:
		{
			this->call_function(ast);
			break;
		}
	}

	// Clean up subtables
	std::vector<std::string> subtables;
	for(auto& it : this->subtables)
	{
		subtables.push_back(it.first);
	}
	for(int i = 0; i < subtables.size(); i++)
	{
		delete this->subtables[subtables[i]];
		this->subtables.erase(subtables[i]);
	}
}
