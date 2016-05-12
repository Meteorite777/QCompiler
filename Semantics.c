/* Semantics.c
   Support and semantic action routines.
   
*/

#include <string.h>

#include "Semantics.h"

/* The main identifier symbol table */
struct SymTab * IdentSymTab;

char * BaseTypeNames[2] = { "Int", "Char"};

/* Semantics support routines */
void
InitSemantics()
{
  IdentSymTab = CreateSymTab(100);
}

void
ListSymTab()
{ int i;
  struct SymEntry * AnEntry;
  char buf[256];
  
  // Could shift this to go to the listing file instead of stdout.
  fprintf(stdout,"\nSymbol Table Contents\n");
  fprintf(stdout,"Num         ID Name  Type\n");
  AnEntry = FirstEntry(IdentSymTab);
  i = 1;
  while (AnEntry) {
    struct Attr * attr = (struct Attr *) GetAttr(AnEntry);
      fprintf(stdout,"%3d %15s %5s\n", i, GetName(AnEntry), BaseTypeNames[attr->TypeDesc->BaseType]);
    i++;
    AnEntry = NextEntry(IdentSymTab,AnEntry);
  }
}

/* Semantic Actions */
struct IdList *   
ProcUndId(char * IdText)
{
	SymEntry* foundEntry = NULL;
	if(EnterName(IdentSymTab, IdText, &foundEntry)){
		char msg[MAXLINE];
		sprintf(msg,"ERROR \"Identifier being declared has been found to already be in the symbol table.\" token: \"%s\"", yytext);
		PostMessage(GetCurrentColumn(), msg);
		exit(-1);
	}
	IdList* TheList = malloc(sizeof(IdList));
	TheList->TheEntry = foundEntry;
	TheList->Next = NULL;
	free(IdText);
	return(TheList);
		
}

struct IdList *   
ChainUndId(struct IdList * TheList, char * IdText) 
{
	// I just made the choice pop the new ID onto the front of the list. I could traverse to end to preserve order.
	IdList* newIdList = ProcUndId(IdText);
	newIdList->Next = TheList;
	return newIdList;
}

//Updates Symbol Table with type of ids in list, also reads IdList and generates assembly instruction sequence .
struct InstrSeq *
ProcDecl(struct IdList * TheList, struct TypeDesc * TheType) 
{
	InstrSeq* instrHead = NULL;
	InstrSeq* newInstr = NULL;
	int firstPass = 0;
	
	while(TheList != NULL){
		SymEntry* tableEntry = FindName(IdentSymTab, TheList->TheEntry->name);
		Attr* identAttr = malloc(sizeof(Attr));
		char* tmp = malloc(strlen(tableEntry->name) + 1);
		sprintf(tmp, "_%s", tableEntry->name);
		
		identAttr->TypeDesc = TheType;
		identAttr->Label = tmp; 
		SetAttr(tableEntry, identAttr);
		free(identAttr);
		identAttr = NULL;

		newInstr = GenInstr( ((struct Attr *)GetAttr(tableEntry))->Label, ".word", "0", NULL, NULL);
		AppendSeq(instrHead, newInstr);
		/* Since the head of the list is first generated here it's hard to keep a reference to it above without 
		   clobbering it. After the head gets allocated I save a reference to it down for return below. */
		if(instrHead == NULL){
			instrHead = newInstr;
		}

		TheList = TheList->Next;
	}
	return instrHead;
}

struct TypeDesc * 
ProcTypeDesc(enum BaseTypes TheBaseType) 
{
	TypeDesc* newTypeDesc = malloc(sizeof(TypeDesc));
	newTypeDesc->BaseType = TheBaseType;
	return newTypeDesc;
}

void
Finish(struct InstrSeq * DeclsCode, struct InstrSeq * BodyCode)
{
  printf("Finish\n");
  
  ListSymTab();
  struct InstrSeq * dataSection, * textSection, * finalCode;
  
  textSection = GenInstr(NULL,".text",NULL,NULL,NULL);
  //AppendSeq(textSection, BodyCode);
  
  dataSection = GenInstr(NULL,".data",NULL,NULL,NULL);
  AppendSeq(dataSection, DeclsCode);

  AppendSeq(dataSection, textSection);
  
  WriteSeq(dataSection);
  
  // write code sequences out to the assembly file
  // this will boilerplate for text and data segment headers and alignment information
  
  // there should be only one call to WriteSeq with entire program since WriteSeq closes the
  // file handle when done
  
  // Choice: generate data segment statements for variables as they are declared, pass up through
  // DeclsCode and concatenate with remainder of code or enumerate symbol table now and generate
  // data segment statement for variables
  
}
