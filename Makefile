LOADLIBES = -ll -ly
CFLAGS = -g  -std=gnu11

#==========================
# Make rule syntax
# target: dependencies
# 	rule to build target from dependencies
#
# - no rule necessary if make already knows (i.e. .o from .c)
# - sometime no dependencies (i.e. clean target)

#==========================
# Symbol Table Project
SymTab.o: SymTab.c SymTab.h

SymTabDriver: SymTabDriver.o SymTab.o

symtest: SymTabDriver
	./SymTabDriver SymData.txt


#===========================
# IOMngr Project
IOMngr.o: IOMngr.c IOMngr.h

IOMngrDriver: IOMngrDriver.o IOMngr.o

iotest1: IOMngrDriver
	./IOMngrDriver IOMngrSource IOMngrListing

iotest2: IOMngrDriver
	./IOMngrDriver IOMngrSource


#===========================
# Scanner Project
ScannerDriver.o: ScannerDriver.c Scanner.h Tokens.h 

ScannerDriver: ScannerDriver.o Scanner.o SymTab.o IOMngr.o

scantest: ScannerDriver
	./ScannerDriver

scanextest: ScanExample
	./ScanExample


# Other
clean:
	rm *.o SymTabDriver IOMngrDriver ScannerDriver
