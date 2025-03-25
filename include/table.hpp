#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include "script.hpp"

namespace db
{
namespace table
{
typedef std::string value_t;
typedef std::vector<value_t> row_t;
typedef std::vector<row_t> table_t;

class Table
{
public:
    table_t table;

    row_t* where(std::string condition);
};
} // namespace table
} // namespace db
