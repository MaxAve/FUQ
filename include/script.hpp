#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace db
{
namespace script
{
// Operand types
enum OperandType
{
    COLUMN,
    VALUE
};

// Operation types
enum OperationType
{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    SQRT,
    EQUALS,
    NEQUALS,
    GREATER,
    LESS,
    GREATEREQUALS,
    LESSEQUALS,
    OR,
    AND,
    NOT,
};

extern const std::unordered_map<std::string, OperationType> operators;

// Operand
typedef struct
{
    OperandType type;
    std::string value;
} Operand;

// Operation
typedef struct
{
    std::vector<Operand> operands;
    OperationType optype;
} Operation;
} // namespace script
} // namespace db
