/** Author: Nicholas Frank
	Date: 2/8/2015
	Summary: This is a symbol table implementation for the construction of a basic compiler with some of the initial unit testing code
	left in but commented out in in the event of future debugging.
**/
#define _GNU_SOURCE 1
#include "SymTab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Old print statements used for testing low level functions. Commented out in case I made a mistake.
/*int main(int argc, char **argv){
	const char *testName = "Lee";
	const char *testName2 = "Fred";
	printf("Test Name to Hash: %s\n", testName);
	SymTab *myTable = CreateSymTab(5);
	printf("Initial Hash Num: %d\n", compHash(testName, myTable->size));
	printf("Testing Find Name on Empty Table: %p\n\n", FindName(myTable, testName));
	
	SymEntry *enteredEntry = (SymEntry*) malloc(sizeof(SymEntry));
	printf("Found Name on Entering %s?: %d\n",testName, EnterName(myTable, testName, &enteredEntry));
	
	if(enteredEntry != NULL){
		printf("Name Value Found/Entered: %s\n", enteredEntry->name);
	}
	
	printf("Testing Find Name on Single Test Entry: %p\n", FindName(myTable, testName));
	

	printf("Found Name on Entering %s?: %d\n", testName2, EnterName(myTable, testName2, &enteredEntry));
	if(enteredEntry != NULL){
		printf("Name Value Found/Entered: %s\n", enteredEntry->name);
	}
	
	printf("Testing Find Name on Second Test Entry: %p\n", FindName(myTable, testName2));
	
	printf("Finding first value in table: %p\n", FirstEntry(myTable));
	printf("Finding next value in table: %p\n", NextEntry(myTable, FirstEntry(myTable)));
	printf("Finding next value after last value (null): %p\n", NextEntry(myTable, NextEntry(myTable, FirstEntry(myTable))));
	
	return 0;
}*/


struct SymTab *CreateSymTab(int size){
	SymTab *symTable = (SymTab*) malloc(sizeof(SymTab));
	symTable->size = size;
	symTable->contents = (SymEntry**) malloc(sizeof(SymEntry *)*size);
	
	//initialize array of SymEntries
	int i;
	for(i = 0; i < size; i++){
		SymEntry *nullEntry = (SymEntry*) malloc(sizeof(SymEntry));
		nullEntry->name = NULL;
		nullEntry->attributes = NULL;
		nullEntry->next = NULL;
		symTable->contents[i] = nullEntry;
	}
	return symTable;
}

void DestroySymTab(struct SymTab *aTable){
	int size = aTable->size;
	
	int i;
	for(i = 0; i < size; i ++){
		SymEntry *currRowEntry = aTable->contents[i];
		while(currRowEntry != NULL){
			//Preserve reference to the next entry
			SymEntry *nextEntry = currRowEntry->next;
			//Free reference to current entry
			free(currRowEntry->name);
			free(currRowEntry);
			currRowEntry = nextEntry;
		}
	}
	free(aTable->contents);
	free(aTable);
}

struct SymEntry *FindName(struct SymTab *symTable, const char * name){
	int hashVal = compHash(name, symTable->size);
	//printf("Hash Num2: %d\n", hashVal);
	
	SymEntry *thisEntry = symTable->contents[hashVal];
	//printf("FindName Name: %s\n", thisEntry->name);
	
	//Check if the first symEntry has a name, if not then there is nothing in that row.
	if( thisEntry->name == NULL){
		return NULL;
	}
	else{
		//If there is a name, then loop through comparing names.
		while( thisEntry->name != NULL){
			//Check if names are equal
			if(strcmp(thisEntry->name, name) == 0){
				return thisEntry;
			}
			// Make progress through row.
			if(thisEntry->next != NULL){
				thisEntry = thisEntry->next;
			}
			else{
				return NULL;
			}
			
		}
	}
	// If we hit the end of the row without a match return NULL.
	return NULL;
}

bool EnterName(struct SymTab *symTable, const char *name, struct SymEntry **addedEntry){
	//Try to find the name with FindName() function.
	SymEntry *foundEntry = FindName(symTable, name);
	
	// If it didn't find the name we should enter it into the table.
	if(foundEntry == NULL){
		// Hash the name to find the row where it should be and grab the first entry.
		int hashVal = compHash(name, symTable->size);
		foundEntry = (symTable->contents)[hashVal];

		// If the first entry of that row is empty just place it there, otherwise traverse to end of row and create new entry.
		if(foundEntry->name == NULL){
			// strdup mallocs space for the required string and appends a null character to the end.
			foundEntry->name = strdup(name);
			*addedEntry = foundEntry;
		}
		else{
			// Progress to the end of the row.
			while (foundEntry->next != NULL){
				foundEntry = foundEntry->next;
			}
			
			// We are now at the last filled entry so we need to make a new one and set it to this->next
			SymEntry *newEntry = (SymEntry*) malloc(sizeof(SymEntry));
			newEntry->name = strdup(name);
			newEntry->attributes = NULL;
			newEntry->next = NULL;
			foundEntry->next = newEntry;
			newEntry->name = strdup(name);
			// Mark the added entry for passback to callee function and return false to signify the name wasn't found.
			*addedEntry = newEntry;
		}
		
		return false;
	}
	else{
		//printf("%s was found in the table...\n", name);
		*addedEntry = foundEntry;
		return true;
	}
}

struct SymEntry *FirstEntry(struct SymTab *aTable){
	int size = aTable->size;
	
	int i;
	for(i = 0; i < size; i ++){
		SymEntry *currRowEntry = aTable->contents[i];
		if(currRowEntry->name != NULL){
			return currRowEntry;
		}
	}
	return NULL;
}

struct SymEntry *NextEntry(struct SymTab *aTable, struct SymEntry *anEntry){
	//Check if the next entry has data, if so return it. Else we go to the next row in the table.
	if(anEntry->next != NULL){
		return anEntry->next;
	}
	//Sanity Check. This will raise errors if we are passed an empty entry.
	if(anEntry->name == NULL){
		fprintf(stderr, "Error: Inputted SymEntry has no name value. Cannot compute a hash.\n");
		exit(-1);
	}
	
	int hashVal = compHash(anEntry->name, aTable->size);
	int size = aTable->size;
	
	//If we have no next and our hash value is the last row of the table then we are at the end of the last row of the table already.
	if(hashVal == size - 1){
		return NULL;
	}
	
	// We know we are at end of current row, so increment to next row and loop.
	hashVal++;
	int i;
	for(i = hashVal; i < size; i++){
		SymEntry *currRowEntry = aTable->contents[i];
		if(currRowEntry->name != NULL){
			return currRowEntry;
		}
	}
	return NULL;
}

void SetAttr(struct SymEntry *anEntry, void *attributes){
	const int attrSize = sizeof(int) * 2;
	//printf("Sizeof: %d\n", attrSize);
	if(anEntry->attributes == NULL){
		anEntry->attributes = malloc(attrSize);
	}
	memcpy(anEntry->attributes, attributes, attrSize);
	
}

void *GetAttr(struct SymEntry *anEntry){

	return anEntry->attributes;
}

const char *GetName(struct SymEntry *anEntry){
	return anEntry->name;
}

struct Stats *Statistics(struct SymTab *aTable){
	int rowLen;
	int i;
	
	Stats *tableStats = (Stats*) malloc(sizeof(Stats));
	tableStats->minLen = 2147483647; //Max Integer 
	tableStats->maxLen = 0;
	tableStats->avgLen = 0;
	tableStats->entryCnt = 0;
	
	i = 0;
	for(i = 0; i < aTable->size; i ++){
		rowLen = 0;
		SymEntry *currRowEntry = aTable->contents[i];
		while(currRowEntry->name != NULL){
			tableStats->entryCnt++;
			rowLen++;
			if(rowLen < tableStats->minLen){
				tableStats->minLen = rowLen;
			}
			if(rowLen > tableStats->maxLen){
				tableStats->maxLen = rowLen;
				//printf("max -- %d       row -- %d\n", tableStats->maxLen, rowLen);
			}
			if(currRowEntry->next != NULL){
				currRowEntry = currRowEntry->next;
			}
			else{
				break;
			}
			
		}
		tableStats->entryCnt = tableStats->entryCnt + rowLen;
	}
	
	tableStats->avgLen = tableStats->entryCnt / aTable->size;
	return tableStats;
}

int compHash(const char *aString, int tableSize){
	int i;
	int hashSum = 0;
	
	if(aString == NULL){
		fprintf(stderr, "ERROR: compHash was passed a null string.");
		exit(-1);
	}
	
	for(i = 0; i < strlen(aString); i++){
		hashSum += aString[i];
	}
	return hashSum % tableSize;
}
