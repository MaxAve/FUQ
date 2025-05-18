#pragma once

#include <iostream>
#include <vector>

namespace db
{
namespace utils
{
bool is_int(std::string str);

bool is_float(std::string str);

bool is_number(std::string str);

bool is_greater(std::string a, std::string b);

// Custom quicksort implementation (because our teacher hates the standard library apparently)
std::vector<std::vector<std::string>> quicksort_table(std::vector<std::vector<std::string>> table, int column, bool ascend);
}
}