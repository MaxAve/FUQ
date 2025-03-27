#pragma once

#include <iostream>
#include "parser.hpp"

namespace db
{
namespace interpreter
{
enum ASTTokenType
{
    VALUE,
    FUNCTION_CALL,
    EXPRESSION,
};

class AST
{
public:
    ASTTokenType type;
    token_t value;
    std::vector<AST> children;

    AST(const std::vector<token_t>& tokens);

    void print(int depth=0);
};

bool is_expression(const std::vector<token_t>& tokens);

void run(std::string line);
} // namespace interpreter
} // namespace db
