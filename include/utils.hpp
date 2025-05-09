#pragma once

#include <iostream>

namespace db
{
namespace utils
{
bool is_int(std::string str);

bool is_float(std::string str);

bool is_number(std::string str);

bool is_greater(std::string a, std::string b);
}
}