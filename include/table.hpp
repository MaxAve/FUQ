#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include "utils.hpp"

namespace db
{
namespace table
{
typedef struct
{
    bool ascending;
    size_t column_index;
} SortRule;

class Table
{
public:
	std::vector<std::vector<std::string>> table; // TODO optimize

	SortRule sort_rule;
    
	Table(std::string file); // load

	size_t get_col_index(std::string col_name);
	
	void print();
	void save(std::string path);
	void set(std::string col, std::string value);
	void sort();
};

class SubTable
{
public:
	Table* target;
	std::vector<size_t> rows;
	
	SubTable(Table* target);

	void print();
	void save(std::string path);
	void sort();
};
} // namespace table
} // namespace db
