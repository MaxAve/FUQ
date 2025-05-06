# FUQ - Functional Unstructured Query
FUQ is a simple query language for a simple database that I made as a projec for school.\
Note: this is was a pretty rushed proof of concept and I can assure you if you look into the code for just a moment you will find more than enough questionable shortcuts, design choices, and a plethora of bugs. Enjoy!
### Design goals
The goal of FUQ is to be a query language (similar to SQL) that uses a syntax resembling function calls.
### Example
Here is a query that will load a CSV file and replace the first column in every second entry with the value "FUQ!"\
```table.csv```:
```csv
col1,col2,col3,col4
a,b,c,d
e,f,g,h
i,j,k,l
m,n,o,p
q,r,s,t
```
```query.fuq```:
```py
load("table.csv", table)
prints("Before:")
printt(table)
set(filter(table, [INDEX] % 2 == 0), col1, "FUQ!")
prints("After:")
printt(table)
```
The ```filter``` function also allows expressions to retrieve and comapre column values. The below code will replace the ```k``` in column ```col3``` with ```FUQ!```\
```query.fuq```:
```py
load("table.csv", table)
prints("Before:")
printt(table)
set(filter(table, [col3] == "k"), col3, "FUQ!")
prints("After:")
printt(table)
```
The example only shows 5 of the functions FUQ has (```load```, ```prints```, ```printt```, ```set```, ```filter```). To get a list of all functions, type ```flist``` into the interpreter. To get more detailed infos on each function, call ```help(FUNCTION)```.