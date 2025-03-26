#include <script.hpp>

const std::unordered_map<std::string, db::script::OperationType> db::script::operators = {
	{"+", ADD},
	{"-", SUB},
	{"*", MUL},
	{"/", DIV},
	{"^", POW},
	{"sqrt", SQRT},
	{"==", EQUALS},
	{"!=", NEQUALS},
	{">", GREATER},
	{"<", LESS},
	{">=", GREATEREQUALS},
	{"<=", LESSEQUALS},
	{"or", OR},
	{"and", AND},
	{"not", NOT},
	{"(", FUNC_PARAM_START},
	{")", FUNC_PARAM_END},
	{",", FUNC_PARAM_DELIMITER},
};