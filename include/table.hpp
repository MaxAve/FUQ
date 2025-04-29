#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>

namespace db
{
namespace table
{
class Table
{
public:
	std::vector<std::vector<std::string>> table; // TODO optimize
    
	Table(std::string file); // load
	
	void print();
	void save(std::string path);
	void set(std::string col, std::string value);
};

class SubTable
{
public:
	Table* target;
	std::vector<size_t> rows;
	
	SubTable(Table* target);

	void print();
	void save(std::string path);
	//void set(std::string col, std::string value);
};
} // namespace table
} // namespace db
