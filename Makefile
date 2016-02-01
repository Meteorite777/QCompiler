CFLAGS= -g -std=gnu11 

#==========================
# Make rule syntax
# target: dependencies
# 	rule to build target from dependencies
#
# - no rule necessary if make already knows if (i.e. .o from .c)
# - sometime no dependencies (i.e. clean target)

#==========================
# Symbol Table Project

SymTab.o: SymTab.c SymTab.h

SymTabDriver: SymTabDriver.o SymTab.o

symtest: SymTabDriver
	./SymTabDriver SymData.txt




# Other
clean:
	rm *.o SymTabDriver 
