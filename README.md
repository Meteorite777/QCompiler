# CS 442 - Compiler Construction
## SymTab Assignment Respository

This repository initially contains the SymTab Assignment files: 

- SymTabDriver.c 
 - The main test driver for the assignment. This driver 
    - creates a symbol table
    - reads names and attribute values from stdin
  - enters the names into the table
  - displays the table contents and statistics
  - creates a copy of the original table
  - destroys the original table
  - display the copy contents and statistics
  - creates a copy of the copy
  - destroys the first copy
  - displays the second copy contents and statistics
  - verifies counts in the attributes structure
  - frees all attribute structures in the second copy
  - destroys the second copy
- SymTab.h 
 - The include file defining the required structures and functions for the implementation. 
- SymData.txt 
 - A file of test data. This provides a minimal test. Your submission will be tested with a different data file. 
- Makefile 
 - The Makefile containing the dependency rules as well as "clean" and "test" targets.

## The Assignment

Create the SymTab.c file and implement the functions defined in SymTab.h. The implementation will use a hash table structure containing singly linked lists of names with equal hash values along with their attribute structures. The project can be tested with "SymTabDriver SymData.txt" or "make symtest" is a shorthand for this. 

## What To Do

- In GitLab, fork this project to make a copy under your GitLab account, it will still be called "Assignment"
- Clone _your_ copy of the project to your development machine. 
- Implement SymTab.c and test. 
- When complete, 
 - "make clean" to remove object files and executables
 - use git to add and commit your changes to your local repository
 - use git to push the project back up to your GitLab account
 - add me as a "reporter" member of your project repository
 - create an issue on your project repository, listing me as assignee, with a title of "Submit for grading"
