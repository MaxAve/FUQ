# FUQ - Functional Unstructured Query
FUQ is a simple query language for a simple database that I made as a projec for school.\
Note: this is was a pretty rushed proof of concept and I can assure you if you look into the code for just a moment you will find more than enough questionable shortcuts, design choices, and a plethora of bugs. Enjoy!
### Design goals
The goal of FUQ is to be a query language (similar to SQL) that uses a syntax based on function calls. For a list of all functions, type ```flist``` into the interpreter, to get infos on specific functions, run ```help(<function>)```.
### Usage
Compile the source code using the provided Makefile (the binary will be saved as ```fuq```)\
To run, you can either execute the binary as-is (```./fuq```) to run the interpreter, or run a ready query file via ```./fuq your_query_file_here.fuq```.
The ```examples``` directory contains a table called ```table.csv``` and multiple query files that demonstrate different aspects of the language.