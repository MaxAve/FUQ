#include <script.hpp>

const std::unordered_map<std::string, db::script::OperationType> db::script::operators = {
	{"+", ADD},
	{"-", SUB},
	{"*", MUL},
	{"/", DIV},
	{"^", POW},
	{"%", MOD},
	{"==", EQUALS},
	{"!=", NEQUALS},
	{">", GREATER},
	{"<", LESS},
	{">=", GREATEREQUALS},
	{"<=", LESSEQUALS},
	{"||", OR},
	{"&&", AND},
	{"!", NOT},
	{"(", FUNC_PARAM_START},
	{")", FUNC_PARAM_END},
	{",", FUNC_PARAM_DELIMITER},
};

const std::unordered_map<db::script::OperationType, db::script::OPInfo> db::script::operator_info = {
	{ADD, {4, true}},
	{SUB, {4, true}},
	{MUL, {3, true}},
	{DIV, {3, true}},
	{MOD, {3, true}},
	{POW, {2, true}},
	{EQUALS, {7, true}},
	{NEQUALS, {7, true}},
	{GREATER, {6, true}},
	{LESS, {6, true}},
	{GREATEREQUALS, {6, true}},
	{LESSEQUALS, {6, true}},
	{OR, {12, true}},
	{AND, {11, true}},
	{NOT, {2, false}},
	{FUNC_PARAM_START, {0xff, true}},
	{FUNC_PARAM_END, {0xff, true}},
	{FUNC_PARAM_DELIMITER, {0xff, true}},
};

const std::unordered_map<std::string, db::script::FunctionInfo> db::script::function_infos = {
	{"print", {db::script::FunctionID::PRINT, 1}},
	{"prints", {db::script::FunctionID::PRINTS, 1}},
	{"load", {db::script::FunctionID::LOAD, 2}},
	{"unload", {db::script::FunctionID::UNLOAD, 1}},
	{"set", {db::script::FunctionID::SET, 3}},
	{"filter", {db::script::FunctionID::FILTER, 2}},
	{"insert", {db::script::FunctionID::INSERT, -1}},
	{"erase", {db::script::FunctionID::ERASE, 1}},
};
