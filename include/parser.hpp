#pragma once

#include <iostream>
#include "script.hpp"

typedef std::string token_t;

namespace db
{
namespace parser
{
// Grabs token at the index i; use this to avoid overflows
token_t get(std::vector<token_t> tokens, int i);

// Removes comments from a code line
std::string remove_comments(std::string str);

std::string filter(std::string str, char c, bool ignore_strings=false);

// Normalize provided character
// Example: normalize("aaaaaa bbbb ccc", 'a', false) -> "a bbbb ccc"
std::string normalize(std::string str, char c, bool ignore_strings=false);

// Grabs operator at the provided index
token_t get_operator(std::string str, int i);

// Tokenizes a line of code
std::vector<token_t> tokenize(std::string str);
} // namespace parser
} // namespace db
