#pragma once

#include <iostream>
#include "script.hpp"

namespace db
{
namespace parser
{
std::string normalize(std::string str, char c, bool ignore_strings=false);

std::vector<std::string> split(std::string str, char c);
} // namespace parser
} // namespace db
