#pragma once

#include <iostream>
#include "parser.hpp"

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

bool is_expression(const std::vector<token_t>& tokens);

class InterpreterContex
{
public:
    void run_function(const AST& fcall);

    void run(std::string line);
};
} // namespace interpreter
} // namespace db
