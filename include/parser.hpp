#pragma once

#include <iostream>
#include "script.hpp"

namespace db
{
namespace parser
{
std::string normalize(std::string str, char c, bool ignore_strings=false);

std::string get_operator(std::string str, int i);

std::vector<std::string> tokenize(std::string str);
} // namespace parser
} // namespace db
