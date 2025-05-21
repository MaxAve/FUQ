# FUQ - Functional Unstructured Query
FUQ is a simple query language for a simple database that I made as a projec for school.\
Note: this is was a pretty rushed proof of concept and I can assure you if you look into the code for just a moment you will find more than enough questionable shortcuts, design choices, and a plethora of bugs. Enjoy!
### Design goals
The goal of FUQ is to be a query language (similar to SQL) that uses a syntax based on function calls. For a list of all functions, type ```flist``` into the interpreter, to get infos on specific functions, run ```help(<function>)```.
### Usage
Compile the source code using the provided Makefile (the binary will be saved as ```fuq```)\
To run, you can either execute the binary as-is (```./fuq```) to run the interpreter, or run a ready query file via ```./fuq your_query_file_here.fuq```.\
The ```examples``` directory contains a table called ```table.csv``` and multiple query files that demonstrate different aspects of the language.
## Internal design overview (read at own risk of attaining an anyerism)
### Files
```cli.cpp```: Runs the interpreter in shell\
```help.cpp```: Contains functions that display helpful messages\
```interpreter.cpp```: Where all the magic happens. Contains functions that convert token lists into ASTs and run them\
```main.cpp```: main\
```parser.cpp```: Various functions for parsing and tokenizing code\
```script.cpp```: Defines rules for FUQ-Lang (e.g. operators, functions)\
```table.cpp```: Definitions for ```Table``` and ```SubTable``` classes\
```utils.cpp```: Various utility functions (type conversion, type-independant comparison, sorting, etc.)
### Table
You can guess what this is.
### Sub-Table
A list of pointers each of which point to a single entry in tables. Functions that perform operations on tables can also take a sub-table as parameter and will perform operations on entries that are contained inside the sub-table.
### Lambda
An expression that can be executed on an entire table (or sub-table). They are called lamdas because the expression needs to be saved in order for it to be ran on every single entry in a table.\
Currently lambdas cannot yet execute functions and are limited to expression operators.
### Function
A function is the core of FUQ-Lang. A function can take any number of parameters which can be any of the following:
- Value (string or number)
- Lambda
- Function call (returns a value)\
When a function needs to return a sub-table, it will allocate the sub-table to the ```db::interpreter::Context::subtables``` hashmap and return its ID. A function cannot return a lambda.