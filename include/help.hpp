#pragma once

#include <iostream>
#include <unordered_map>

namespace db
{
namespace help
{
// Usage & description of a function
typedef struct
{
    std::string usage;
    std::string description;
} FunctionInfo;

// Table of name and function infos
extern std::unordered_map<std::string, FunctionInfo> function_info;

// Prints a list of all functions
void list_all_functions();

// Returns infos to a function
void get_function_info(std::string function_name);
}
}