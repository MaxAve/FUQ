#include <help.hpp>

std::unordered_map<std::string, db::help::FunctionInfo> db::help::function_info = {
    {"printt", {"printt(table)", "Prints the contents of [table] to stdout."}},
    {"prints", {"printt(message)", "Prints [message] to stdout."}},
    {"create", {"create(table, columns...)", "Creates an empty table with a column row."}},
    {"load", {"load(path, table)", "Loads a new table from [path] and saves it as [table] in the current context."}},
    {"unload", {"unload(table)", "Unloads a table from the current context."}},
    {"save", {"save(path, table)", "Saves the table [table] as a file specified in [path]."}},
    {"set", {"set(table, column, value)", "Iterates over every row in [table] and sets the value at [column] to [value]. [value] can be either a constant or a lambda."}},
    {"filter", {"filter(table, condition)", "Iterates over every row in [table] and applies [condition] as a lambda. Rows that return 1 (true) will be added to a sub-table and returned."}},
    {"erase", {"erase(table)", "Erases every element in [table]. When a sub-table is passed as a parameter, every row present in the sub-table will be erased from the original table.\nExample:\n\terase(filter(table, [INDEX] % 2 == 0)) # This will erase every second row in the table"}},
    {"sortrule", {"sortrule(table, column, ascending)", "Sets rules for sorting [table]. The table will be sorted by values in [column] in ascending order if [ascending] == 1, otherwise descending."}},
    {"insert", {"insert(table, values...)", "Inserts a new row into [table]. The row will be inserted according to rules defined via the last sortrule call."}},
    {"colinsert", {"colinsert(table, column)", "Inserts a new empty column [column] into [table]"}},
    {"colerase", {"colerase(table, column)", "Removes a column [column] from [table]"}},
    {"join", {"join(table1, table2, sort)", "Appends [table2] into [table1]. [table2] will be inserted sorted if [sort] is set to 1"}},
    {"sort", {"sort(table)", "Sorts [table] according to rules defined via the last sortrule call."}},
    {"help", {"help(function)", "I think you know what this does."}},
};

void db::help::list_all_functions()
{
    std::cout << "printt\nprints\ncreate\nload\nunload\nsave\nset\nfilter\nerase\nsortrule\ninsert\ncolinsert\ncolerase\njoin\nsort\nhelp\n";
}

void db::help::get_function_info(std::string function_name)
{
    if(db::help::function_info.find(function_name) == db::help::function_info.end())
    {
        std::cout << "ERROR: Function \"" << function_name << "\" does not exist.\n";
        return;
    }
    const db::help::FunctionInfo info = db::help::function_info.at(function_name);
    std::cout << "Function:\n\t" << function_name << "\nUsage:\n\t" << info.usage << "\nDescription:\n\t" << info.description << "\n";
}