CFLAGS= -g -std=gnu11 

#==========================
# Symbol Table Project

SymTab.o: SymTab.c SymTab.h

SymTabDriver: SymTabDriver.o SymTab.o

symtest: SymTabDriver
	./SymTabDriver SymData.txt




# Other
clean:
	rm *.o SymTabDriver 
