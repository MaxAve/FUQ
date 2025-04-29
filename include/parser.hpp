#pragma once

#include <iostream>
#include "script.hpp"

typedef std::string token_t;

namespace db
{
namespace parser
{
token_t get(std::vector<token_t> tokens, int i); /* Use this function instead of getting tokens by index to avoid overflows */

std::string remove_comments(std::string str);

std::string filter(std::string str, char c, bool ignore_strings=false);

std::string normalize(std::string str, char c, bool ignore_strings=false);

token_t get_operator(std::string str, int i);

std::vector<token_t> tokenize(std::string str);
} // namespace parser
} // namespace db
