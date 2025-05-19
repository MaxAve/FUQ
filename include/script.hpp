#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace db
{
namespace script
{
enum TokenType
{
    VALUE,
    FUNCTION_CALL,
    EXPRESSION,
};

// Operators
enum OperationType
{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    MOD,
    EQUALS,
    NEQUALS,
    GREATER,
    LESS,
    GREATEREQUALS,
    LESSEQUALS,
    OR,
    AND,
    NOT,
    FUNC_PARAM_START,
    FUNC_PARAM_END,
    FUNC_PARAM_DELIMITER,
};

// Map operator string : operator type
extern const std::unordered_map<std::string, OperationType> operators;

typedef struct
{
    uint8_t precedence;
    bool double_operand;
} OPInfo;

extern const std::unordered_map<OperationType, OPInfo> operator_info;

// Literally every function you will ever need
enum FunctionID
{
	PRINTT,
    PRINTS,
    LOAD,
    CREATE,
	UNLOAD,
    SAVE,
    SET,
    FILTER,
    INSERT,
    ERASE,
    SORT,
    SORTRULE,
    JOIN,
    HELP,
};

typedef struct
{
    FunctionID id;
    int expect_parameters; // Number of parameters expected
} FunctionInfo;

extern const std::unordered_map<std::string, FunctionInfo> function_infos;
} // namespace script
} // namespace db
