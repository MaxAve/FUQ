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
    token_t value;
    std::vector<AST> children;

    AST(std::vector<token_t>& tokens);
};

void run(std::string line);
} // namespace interpreter
} // namespace db
