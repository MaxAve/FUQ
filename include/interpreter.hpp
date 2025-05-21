#pragma once

#include <iostream>
#include <vector>
#include <variant>
#include <cmath>
#include "parser.hpp"
#include "table.hpp"
#include "help.hpp"
#include "utils.hpp"

namespace db
{
namespace interpreter
{
// Abstract Syntax Tree node class
class AST
{
public:
    db::script::TokenType type; // Function, expression or value
    token_t value; // This will only be set if the token is a value
    std::vector<AST> children;

    AST(std::string val);
    AST(const std::vector<token_t>& tokens);

    void print(int depth=0);
};

// Function call struct
typedef struct
{
    db::script::FunctionID fid;      // Type of function being executed
    std::vector<std::string> params; // Parameters passed to the function
    
    // Do we even need this?
    size_t ret;
	size_t stack_index;
} FCall;

// Lambda struct
typedef struct
{
    std::unordered_map<std::string, std::string> params;
    AST code;
} Lambda;

// Interpreter context class
class Context
{
private:
    std::unordered_map<std::string, db::table::Table*> tables;       // Loaded tables
    std::unordered_map<std::string, db::table::SubTable*> subtables; // Loaded sub-tables
    std::vector<Lambda> lambdas;                                     // Lambda stack
    std::vector<FCall> call_stack;                                   // Function call stack
public:
    ~Context();

    uint32_t err = 0; // If not set to 0, the interpreter will stop and not execute any further queries

	void load_table(std::string file, std::string newname);
    void unload_table(std::string name);

    // Call a function from an AST
	std::string call_function(const AST& fcall);

    // Evaluates a lambda and returns the result as a string
    std::string evaluate_lambda(Lambda& lambda);

    // Parses, tokenizes, and runs a line of code
    void run(std::string line);

	// This function was used for debugging
	void print_call_stack();
};

// Returns true if the list of tokens can be interpreted as a string
bool is_string(const std::vector<token_t>& tokens);

// Returns true if the list of tokens can be interpreted as an expression
bool is_expression(const std::vector<token_t>& tokens);
} // namespace interpreter
} // namespace db
