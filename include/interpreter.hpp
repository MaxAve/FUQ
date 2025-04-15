#pragma once

#include <iostream>
#include <vector>
#include <variant>
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

/*enum VarType
{
    INT,
    FLOAT,
    STRING,
    PTR_VEC,
};

typedef struct
{
    VarType dtype;
    std::variant<int64_t, double, std::string, std::vector<uint64_t>> value;
} VMemValue;*/

typedef struct
{
    db::script::FunctionID fid; // Type of function being executed
    std::vector<std::string> params; // Parameters passed to the function
    uint8_t ret;
	int stack_index;
} FCall;

class Context
{
private:
    std::unordered_map<std::string, db::table::Table*> loaded_tables; // Maps table names to their corresponding table objects
    db::table::Table* return_tables[256];
    std::vector<FCall> call_stack;
public:
	Context();
    
	void load_table(std::string file, std::string newname);

    void call_function(const AST& fcall);

    void run(std::string line);

	// Utils
	void print_call_stack();
};

bool is_string(const std::vector<token_t>& tokens);

bool is_expression(const std::vector<token_t>& tokens);
} // namespace interpreter
} // namespace db
