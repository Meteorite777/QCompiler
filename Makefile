LOADLIBES = -ll -lfl 
CFLAGS = -g -std=gnu11 -DYYDEBUG=1

# Symbol Table Project
SymTab.o: SymTab.c SymTab.h

SymTabDriver: SymTabDriver.o SymTab.o

symtest: SymTabDriver
	./SymTabDriver SymData.txt

# IOMngr Project
IOMngr.o: IOMngr.c IOMngr.h

IOMngrDriver: IOMngrDriver.o IOMngr.o

iotest: IOMngrDriver
	./IOMngrDriver IOMngrSource IOMngrListing

# Scanner Project
ScannerDriver.o: ScannerDriver.c Scanner.h Tokens.h 

ScannerDriver: ScannerDriver.o Scanner.o SymTab.o IOMngr.o

scantest: ScannerDriver
	./ScannerDriver

# Recursive Descent Project
RecDescent.o: RecDescent.c Scanner.h RDTokens.h

RecDescent: RecDescent.o RDGrammar.o RDScanner.o RDSupport.o IOMngr.o SymTab.o

rdtest1: RecDescent
	./RecDescent RDSrc-1
rdtest2: RecDescent
	./RecDescent RDSrc-2
rdtest3: RecDescent
	./RecDescent RDSrc-3
rdtest4: RecDescent
	./RecDescent RDSrc-4

# Parser Stage 1 & 2
Parse.o: Parse.c Grammar.h Scanner.h RDTokens.h IOMngr.h 

Parse: Parse.o ParserGrammar.o ParserScanner.o IOMngr.o

parse1:	Parse
	./Parse ParSrc-1
parse2:	Parse
	./Parse ParSrc-2


# Semantics
Q.o: 	Q.c Grammar.h Scanner.l IOMngr.h 
Q:	Q.o SymTab.o IOMngr.o QScanner.o QGrammar.o Semantics.o CodeGen.o
q1:	Q
	./Q q1
q2:	Q
	./Q q2

# Other
testall: symtest iotest scantest rdtest1 rdtest2 rdtest3 rdtest4 parse1 parse2 q1 q2
clean:
	rm *.o SymTabDriver IOMngrDriver ScannerDriver RecDescent Parse Q
