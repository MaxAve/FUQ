#pragma once

#include <iostream>
#include <unordered_map>

namespace db
{
namespace help
{
struct FunctionInfo
{
    std::string usage;
    std::string description;
};

extern std::unordered_map<std::string, FunctionInfo> function_info;

void list_all_functions();

void get_function_info(std::string function_name);
}
}