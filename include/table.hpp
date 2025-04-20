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
};
} // namespace table
} // namespace db
