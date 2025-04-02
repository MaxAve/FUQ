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

enum VarType
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
} VMemValue;

typedef struct
{
    db::script::FunctionID fid;
    std::vector<std::string> params;
    std::string ret;
} FCall;

class Context
{
private:
    std::unordered_map<std::string, db::table::Table*> loaded_tables;
    std::unordered_map<std::string, VMemValue> vmem; // Virtual memory (variables)
    std::vector<FCall> call_stack;
public:
    void load_table(std::string file, std::string newname);

    void call_function(const AST& fcall);

    void run(std::string line);
};

bool is_expression(const std::vector<token_t>& tokens);
} // namespace interpreter
} // namespace db
