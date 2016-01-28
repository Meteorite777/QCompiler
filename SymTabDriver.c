/* SymTabDriver.c

   This program minimally exercises the Symbol Table implementation.

*/
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "SymTab.h"

#define MAX(a,b) ( (a) < (b) ? (b) : (a) )

/* A name's attributes consist of two integers. */
struct Attributes {
  int value1; // read from input, retains max for duplicate names
  int value2; // used to count # of times a name is seen in the input
};

void
ErrorExit(char * msg)
{
  fprintf(stderr,"%s",msg);
  exit(1);
}

void
DisplayTable(struct SymTab * table)
{
  struct SymEntry * anEntry = FirstEntry(table);
  int i = 1;
  while (anEntry) {
    fprintf(stdout,"%3d %20s %5d %5d\n", i,
            GetName(anEntry),
            ((struct Attributes *) GetAttr(anEntry))->value1,
            ((struct Attributes *) GetAttr(anEntry))->value2);
    i++;
    anEntry = NextEntry(table, anEntry);
  }
}

void
DisplayStatistics(struct SymTab * table)
{
  struct Stats * stats = Statistics(table);
  fprintf(stdout,"Statistics min: %d max: %d avg: %d total: %d\n",
          stats->minLen,stats->maxLen,stats->avgLen,stats->entryCnt);
  free(stats);
}

void
FreeAllAttr(struct SymTab * table)
{
  struct SymEntry * anEntry = FirstEntry(table);
  while (anEntry) {
    free(GetAttr(anEntry));
    anEntry = NextEntry(table, anEntry);
  }
}

void
VerifyCounts(struct SymTab * table)
{
  struct SymEntry * anEntry = FirstEntry(table);
  while (anEntry) {
    struct Attributes * attr = GetAttr(anEntry);
    if (attr->value1 != attr->value2) {
      fprintf(stdout,"count for %s not correct\n",GetName(anEntry));
    }
    anEntry = NextEntry(table, anEntry);
  }
}

struct SymTab *
CopyTable(struct SymTab * table, int newSize)
{ struct SymTab * copyTable;
  
  if (!(copyTable = CreateSymTab(newSize))) ErrorExit("Failed to alloc copy table.\n");
  
  struct SymEntry * anEntry = FirstEntry(table);
  while (anEntry) {
    struct SymEntry * copyEntry;
    EnterName(copyTable,GetName(anEntry),&copyEntry);
    SetAttr(copyEntry,GetAttr(anEntry));
    anEntry = NextEntry(table, anEntry);
  }
  return copyTable;
}

int
main(int argc, char * argv[])
{ char *testFileName;
  FILE *fd;
  struct SymTab *theTable, *firstCopy, *secondCopy;
  struct SymEntry *anEntry;
  struct Attributes *anAttr;
  char buffer[16];
  int val1, val2;

  if (argc != 2) {
    fprintf(stderr,"usage: SymTabDriver test-data-file\n");
    exit(1);
  }
  testFileName = argv[1];
  
	fd = fopen(testFileName,"r");
  if (!fd) ErrorExit("Can't open input file.\n");
  
  if (!(theTable = CreateSymTab(5))) ErrorExit("Failed to alloc first table.\n");
  
  /* Read lines consisting of a name string and an integer from std input.
     If name already present increment value2, if new allocate storage
     for attribute structure and fill in fields.
  */
  while (fscanf(fd,"%15s %d",buffer,&val1) != EOF) {
    printf("Find: %15s ",buffer);
    (FindName(theTable,buffer)) ? fprintf(stdout," Present     -")
                                : fprintf(stdout," Not Present -");
    if (EnterName(theTable,buffer,&anEntry)) {
      fprintf(stdout,"  Enter: Present: %15s\n",GetName(anEntry));
      anAttr = (struct Attributes *) GetAttr(anEntry);
      anAttr->value2++;
      anAttr->value1 = MAX(((struct Attributes *) GetAttr(anEntry))->value1,val1);
    }
    else {
      fprintf(stdout,"  Enter: Entered: %15s\n",GetName(anEntry));
      anAttr = malloc(sizeof(struct Attributes));
      anAttr->value1 = val1;
      anAttr->value2 = 1;
      SetAttr(anEntry,anAttr);
    }
  }
  
  fprintf(stdout,"\nContents of Original Table\n");
  DisplayTable(theTable);
  DisplayStatistics(theTable);
  
  if (!(firstCopy = CopyTable(theTable,1))) ErrorExit("Failed to alloc first copy table.\n");
  
  DestroySymTab(theTable);
  
  fprintf(stdout,"\nContents of First Copy Table\n");
  DisplayTable(firstCopy);
  DisplayStatistics(firstCopy);
  
  if (!(secondCopy = CopyTable(firstCopy,100))) ErrorExit("Failed to alloc first copy table.\n");
  
  DestroySymTab(firstCopy);
  
  fprintf(stdout,"\nContents of Second Copy Table\n");
  DisplayTable(secondCopy);
  DisplayStatistics(secondCopy);
  
  VerifyCounts(secondCopy);
  FreeAllAttr(secondCopy);
  DestroySymTab(secondCopy);
  
  return 0;
}