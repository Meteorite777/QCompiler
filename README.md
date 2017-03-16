# CS 442 - Compiler Construction
## Semantics Part I
Authors: Dr. Steven Senger
		 Nicholas C Frank
Date: 5/14/2016

Most of the implementation of a compiler for a pseudo-language called Q which contains most of the functions you would come to expect in a modern day language such as variable evaluation and assignment, nesting, if/then statments, loops, etc. 
This implementation is written in C and compiles the Q into MIPS assembly. If you take a look into the makefile you will see the commands to test the respective parts of the compiler such as the symbol table, scanner, parser, and grammar evaluation. 
Credit to Dr. Steven Senger creating the assignment and most of the stubs, structure, ReadMe', and general direction in writing this compiler.

This repository contains files to be addded to your main project repository to include the semantic processing capabilities to compile programs of the language Q. The repository contains

- Q.c
 - This is the test driver for the assignment. This driver takes one argument, the base name of the source and listing files, and opens these files by appending ".src", ".lst" and ".asm" "to the base name. It then starts the main parse function produced by yacc/bison. 

- QGrammar.y
 - The yacc/bison input describing the grammar rules and actions. This file contains a starting point covers all of the required things and production rules for declaration statements. You will need to complete this file. 

- QTokens.h
 - This file lists defines for symbolic constants for the token types required by the grammar. You will need to extend this as more capabilities are added. 

- CodeGen.h/.c
 - The contains support functions for creating, manipulating and writing assembly code sequences. The implementation is complete.

- Semantics.h/.c
 - This will contain the implementation of the semantic actions used in compilation. It has stubs for the semantic actions used for variable declarations as well as some of the required data structures.

- Makefile
- This file has been updated to include rules for this portion of the project. 

You will need to create the following

- QScanner.l
 - The lex/flex input describing the tokens required by the parser. You can create this from previous versions. It should include the comment nesting capabilities contained in the previous versions.
 
Detailed information on the MIPS instruction set and SPIM simulator can be found at

[http://pages.cs.wisc.edu/~larus/HP_AppA.pdf](http://pages.cs.wisc.edu/~larus/HP_AppA.pdf)

The best choice for a simulator is probably the Mars4_5 Java based simulator. To run the assembly in the simulator use the command "java -jar path-to-jar/Mars4_5.jar nc q.asm".

## The Assignment

- Step 1, 25 pts
  - Implement the necessary semantic actions for variable declarations. Q1.src tests this part. 
  - Notes on most of the things you need to do and remember.
        - QGrammar.y contains grammar rules for declarations and corresponding semantic action functions. Variable declarations are of the form "int x, y, z;". It also lists the required "union" fields and the correspondence of nonterminals to "union" fields.
        - QTokens.h contains token numbers for token types used in QGrammar.y.
        - Semantics.h contains the declaration of the semantic action functions, the symbol table identifier attribute struct, a struct IdList to hold symbol table entry references and a struct TypeDesc to represent a type. 
        - Semantics.c contains some global variables, an init function, a list symbol table function and stubs for the semantic action functions. 
        - struct IdList is used to form a linked list of SymTab entries as identifiers are encountered
        - struct TypeDesc is used to define a declaration type - it may become more complicated in the future
        - struct Attr is used with SymTab entries to carry information pertient to an identifier
        - ProcUndId() takes the token text attached to the ID nonterminal, adds it to the symbol table, creates a struct IdList and returns the struct
        - ChainUndId() joins a new identifier token to an existing list
        - ProcDecl() updates the symbol table to reflect the type for all entries on the identifier entry list. it can also create data segment ".word" statements and return as a struct InstrSeq or delay this to the finish function
        - ProcTypeDesc() builds a type descriptor struct
        - Finish() forms the data segment code sequence and writes this out.

- Step 2, 25 pts
 - Add rules and semantic actions to compile simple expressions and assignment statements. Implement get() and put() functions for I/O of simple data types. Get() should be allowed in place of an identifier in expressions and put() should be considered a statement on its own. These are implemented in assembly with syscalls. Q2.src tests this part. 
 - Notes on most of the things you need to do and remember.
        - Write grammar rules for an assignment statement and make it one of the options for Stmt. AssignStmt should be "LVal ASSIGN_TOK Expr".
        - Expr should be "Term AddOp Expr" or "Term". Term should be "Factor MultOp Term" or "Factor". Factor can be a parenthesized expression, the negation of an expression, an integer literal or and identifier.
        - Create a struct ExprResult which holds both the code sequence that computes the expression and the register number where the result is located when the code executes. This will get added as an option for the "union" and associated with certain nonterminals. 
        - The semantic action for compiling the AddOp will receive an ExprResult for the incoming Term and the Expr. The incoming code sequences are concatenated and an instruction for the operator is generated and added to the end. The registers for the operator instruction come from the incoming ExprResults and a new temp register obtained from the CodeGen module. 
        - For an integer literal an ExprResult is created containing a load immediate ("li") instruction with a new temp register. An identifier produces a load word ("lw").
        - The AssignStmt receives an LVal and an ExprResult, generates a store word ("sw") and appends to the end of the ExprResult code sequence and passes along the instruction sequence (not the whole ExprResult). 
        - CodeGen module usage: AvailTmpReg() returns the number of an available register. TmpRegName(reg#) takes a register number and returns the char string name for use in an instruction (e.g. TmpRegName(3) returns "$t3"). ReleaseTmpReg(reg#) make the register available again. For every call to AvailTmpReg() there must somewhere be a corresponding ReleaseTmpReg() call.
        - In compiling an operator, when the language gets more complex, it will be necessary to check that the operands are of compatible types. This means that the ExprResult struct should also have a field for the result type.
        - The action for get() will generate the instructions for the syscall to read an integer and put the result in a new temp register. It will return an ExprResult.
        - The put() statement will generate the instruction for the syscall to write an integer. There are syscalls to print an integer and to print a character. 
        - ChrLit should allow things like 'a' and '\n'.

- Step 3, 25 pts
 - Add rules and semantic actions for "if/then/else" and "while" loops. Q3.src tests this part. 
 - Notes on most of the things you need to do and remember
        - need a CondResult struct similiar to ExprResult, remembers the code sequence and the label used in the conditional branch instruction
        - conditional branch is opposite of the comparison operator, instructions are bge, bg, ble,bl, bne, beq
        - common syntax does allow for dangling else, yacc gives shift/reduce warning but prefers shift over the reduce which effectively ties an else to the inner if
        - if/then/else uses label generated by condition as first thing following then sequence, when an else clause is present, then sequence ends with branch to new label which follows the else sequence and the condition label comes at the beginning of the else clause
        - while needs label at start of condition instruction sequence, conditional branch label follows the body, the body ends with a branch to the top of the loop

- Step 4, 25 pts
 - Add rules and semantic actions to allow string literals in put() statements. Q4.src tests this part. You should also create one test program of your own (to be called q5.src) which exercises all of these features.  
 - Notes on most of the things you need to do and remember
        - use a separate symbol table to hold string literals, need different attribute record for string literals than the one used for identifiers, for string literals the attribute remembers the label used for the string literal in the data section

## What To Do

- Do not fork this repository on Gitlab, instead
- On your development machine, in a location different than your assignment repository, clone this project. Your assignment repository will continue to accumulate all files for the project. 
- Copies the files from this repository to your assignment repository.
- Discard the clone of this repository.
- Implement the required capabilities. 
- When complete, "make clean" to remove object files and executables, use git to add and commit your changes and push the project back up to your repository. Rememger to create an issue in Gitlab with me as the assignee indicating that your submission is ready for grading. 
