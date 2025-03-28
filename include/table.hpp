#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <string>
#include <variant>

typedef std::variant<int64_t, double, std::string> db_value_t;
typedef std::vector<db_value_t> db_entry_t;

namespace db
{
namespace table
{
class Table
{
public:
    std::unordered_map<std::string, size_t> col_names;
    std::unordered_map<std::string, db_entry_t> table;
};
} // namespace table
} // namespace db
