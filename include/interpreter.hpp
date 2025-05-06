#pragma once

#include <iostream>
#include <vector>
#include <variant>
#include <cmath>
#include "parser.hpp"
#include "table.hpp"

namespace db
{
namespace interpreter
{
class AST
{
public:
    db::script::TokenType type;
    token_t value;
    std::vector<AST> children;

    AST(std::string val);
    AST(const std::vector<token_t>& tokens);

    void print(int depth=0);
};

typedef struct
{
    db::script::FunctionID fid; // Type of function being executed
    std::vector<std::string> params; // Parameters passed to the function
    uint8_t ret;
	int stack_index;
} FCall;

typedef struct
{
    std::unordered_map<std::string, std::string> params;
    AST code;
} Lambda;

class Context
{
private:
    std::unordered_map<std::string, db::table::Table*> tables;
    std::unordered_map<std::string, db::table::SubTable*> subtables;
    std::vector<Lambda> lambdas;
    std::vector<FCall> call_stack;
public:
	void load_table(std::string file, std::string newname);
    void unload_table(std::string name);

	std::string call_function(const AST& fcall);

    std::string evaluate_lambda(Lambda& lambda);

    bool is_int(std::string str);

    bool is_float(std::string str);

    bool is_number(std::string str);

    void run(std::string line);

	// Utils
	void print_call_stack();
};

bool is_string(const std::vector<token_t>& tokens);

bool is_expression(const std::vector<token_t>& tokens);
} // namespace interpreter
} // namespace db
