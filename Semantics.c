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

struct Attr * castAttr(void * castee){
	return (struct Attr*) castee;

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
	IdList * TheList = malloc(sizeof(IdList));
	//Don't need to malloc here because we will create memory for it in ChainUndId
        //TheList->Next = malloc(sizeof(IdList));
	// I believe we shouldn't need to malloc memory for Entry as we are just grabbing it's address from symtab and won't free it until the end.
	TheList->TheEntry = foundEntry;
	TheList->Next = NULL;
	free(IdText);
        IdText = NULL;
	return(TheList);
		
}

struct IdList *   
ChainUndId(struct IdList * TheList, char * IdText) 
{
	// I traverse to end of TheList and append the newIDList to preserve order.
	struct IdList * tmpList = TheList;
  	while (tmpList->Next) tmpList = tmpList->Next;
	IdList* newIdList = ProcUndId(IdText);
	tmpList->Next = newIdList;
	return TheList;
}

//Updates Symbol Table with type of ids in list, also reads IdList and generates assembly instruction sequence .
struct InstrSeq *
ProcDecl(struct IdList * TheList, struct TypeDesc * TheType) 
{
	struct InstrSeq* instrHead = NULL;
	struct InstrSeq* newInstr = NULL;
	
	while(TheList != NULL){
		// Find the identifier in the symbol table as we have to update its empty Attr field to contain it's Label and Type.
		SymEntry* tableEntry = FindName(IdentSymTab, TheList->TheEntry->name);
		Attr* identAttr = malloc(sizeof(Attr));
		//identAttr->TypeDesc = malloc(sizeof(TypeDesc));

		char* tmp = malloc(strlen(tableEntry->name) + 1);
		sprintf(tmp, "_%s", tableEntry->name);
		//printf("strlen of label copy: %d\n", (int)strlen(tmp));
		
		//SetAttr fields. SetAttr does a memcpy 
		identAttr->TypeDesc = TheType;
		identAttr->Label = tmp; 
		SetAttr(tableEntry, identAttr);
		
		//printf("SET ATTR TEST: %d Attr Size: %d \n void * size: %d", ((struct Attr *)GetAttr(tableEntry))->TypeDesc->BaseType, sizeof(struct Attr), sizeof(void*) );

		//Generate mips memory allocation instr for the for the identifiers. This is the identifier label to store it at, the type, and value.
		// For .word it stores 'n' contiguous .words in memory, however we only have up to three arguments so I believe it's capped at 3.
		newInstr = GenInstr( ((struct Attr *)GetAttr(tableEntry))->Label, ".word", "0", NULL, NULL);
		AppendSeq(instrHead, newInstr);
		/* Since the head of the InstrSeq list is first generated above here it's hard to keep a reference to it above without 
		   clobbering it. After the head gets allocated I save a reference to it for return down below. */
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

struct ExprResult * ProcExpr(struct ExprResult * Operand1, char * Operator, struct ExprResult * Operand2){
	
	//Malloc new ExprResult struct to hold new and contracted code sequence. 
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = malloc (sizeof(TypeDesc));
	TheResult->TheInstrSeq = Operand1->TheInstrSeq;
	//Error check for compatible types.
	if(Operand1->TheType->BaseType != Operand2->TheType->BaseType){
		fprintf(stderr, "ERROR: Incompatible types: Operand 1: %s --- Operand 2: %s for evaluation.", BaseTypeNames[(int)Operand1->TheType->BaseType], BaseTypeNames[(int)Operand2->TheType->BaseType]);
		exit(-1);
	}
	else{
		TheResult->TheType->BaseType = Operand1->TheType->BaseType;
	}
	AppendSeq(TheResult->TheInstrSeq, Operand2->TheInstrSeq);
	

	//Get new register to store combined result.
	TheResult->TheResultReg = AvailTmpReg();
	struct InstrSeq * newInstr;

	//Determine which new opcode and instruction sequence to generate.
	//printf("**************************************************OPERATOR: %s", Operator); 
	if(*Operator == '+'){
		newInstr = GenInstr(NULL, "add", TmpRegName(TheResult->TheResultReg), TmpRegName(Operand1->TheResultReg), TmpRegName(Operand2->TheResultReg));
	} 	
	else if(*Operator == '-'){
		newInstr = GenInstr(NULL, "sub", TmpRegName(TheResult->TheResultReg), TmpRegName(Operand1->TheResultReg), TmpRegName(Operand2->TheResultReg));
	}
	else if(*Operator =='*'){
		newInstr = GenInstr(NULL, "mul", TmpRegName(TheResult->TheResultReg), TmpRegName(Operand1->TheResultReg), TmpRegName(Operand2->TheResultReg));
	}
	else if(*Operator =='/'){
		newInstr = GenInstr(NULL, "div", TmpRegName(TheResult->TheResultReg), TmpRegName(Operand1->TheResultReg), TmpRegName(Operand2->TheResultReg));
	}
	
	AppendSeq(TheResult->TheInstrSeq, newInstr);
	//Remember to free ExprResults
	ReleaseTmpReg(Operand1->TheResultReg);
	ReleaseTmpReg(Operand2->TheResultReg);
	free(Operand1);
	free(Operand2);
	
	return TheResult;
}

struct ExprResult * ProcFactorIntLit(char * IntLit){
	//Malloc space for new ExprResult
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = malloc (sizeof(TypeDesc));
	//Get new register to store combined result.
	TheResult->TheResultReg = AvailTmpReg();
	TheResult->TheType->BaseType = 0;
	
	TheResult->TheInstrSeq = GenInstr(NULL, "li", TmpRegName(TheResult->TheResultReg), IntLit, NULL);

	return TheResult;
}

struct ExprResult * ProcFactorChrLit(char * ChrLit){
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = malloc (sizeof(TypeDesc));
	//Get new register to store combined result.
	TheResult->TheResultReg = AvailTmpReg();
	TheResult->TheType->BaseType = 1;
	
	//Check length of incoming string to see if it is an escaped control character.
	char* asciiString = malloc(sizeof(char)*32);
	char c = ChrLit[1];
	int ascii = (int) c;
	if(strlen(ChrLit) == 5){
		switch(c) {
			case '\"':
				ascii = 34;
			case '\'':
				ascii = 39 ;
			case '\\':
				ascii = 92 ;
			case '\a':
				ascii = 7;
			case '\b':
				ascii = 8;
			case '\t':
				ascii = 9;
			case '\n':
				ascii = 10 ;
		}
	}
	//printf("^^^^^^^^^^^^^^^^^asciiString before sprintf: %s\n", asciiString);
	sprintf(asciiString, "%d", ascii);
	//printf("char * ChrLit: %s, char c: %c, int ascii: %d, ascii String: %s\n", ChrLit, c, ascii, asciiString); 
	TheResult->TheInstrSeq = GenInstr(NULL, "li", TmpRegName(TheResult->TheResultReg), asciiString, NULL);

	return TheResult;
}

struct ExprResult * ProcFactorNeg(struct ExprResult * Factor){
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = Factor->TheType;
	InstrSeq * newInstr;
	//Get new register to store new negated result.
	TheResult->TheResultReg = AvailTmpReg();
	
	//Generate code sequence to negate value in the Factor register and store it in the new TheResult register.
	newInstr = GenInstr(NULL, "neg", TmpRegName(TheResult->TheResultReg), TmpRegName(Factor->TheResultReg), NULL);
	
	//Append code sequence and free old temporary registers.
	AppendSeq(Factor->TheInstrSeq, newInstr);
	TheResult->TheInstrSeq = Factor->TheInstrSeq;
	ReleaseTmpReg(Factor->TheResultReg);
	//free(Factor->TheInstrSeq);
	free(Factor);

	return TheResult;
}

struct ExprResult * ProcFactorId(char * Ident){
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = malloc (sizeof(TypeDesc));
	//Get new register to store combined result.
	TheResult->TheResultReg = AvailTmpReg();
	
	// Load data in label/identifier from memory into a temporary register.
	Attr * symEntryAttr = GetAttr(FindName(IdentSymTab, Ident));
	TheResult->TheType = symEntryAttr->TypeDesc;
	TheResult->TheInstrSeq = GenInstr(NULL, "lw", TmpRegName(TheResult->TheResultReg), symEntryAttr->Label, NULL);

	return TheResult;
}

char * ProcLVal(char* Ident){
	Attr * symEntryAttr = GetAttr(FindName(IdentSymTab, Ident));
	//printf("%s\n", symEntryAttr->Label);
	return symEntryAttr->Label;
}

struct InstrSeq * ProcAssign(char* TheLabel, struct ExprResult * TheValue){
	InstrSeq * newInstr = malloc(sizeof(InstrSeq));
	newInstr = NULL;

	//Strip the prepended '_' off label for lookup in symbol table. They are stored without '_'. 
	char* cleanLabel = malloc(strlen(TheLabel));
	memmove(cleanLabel, TheLabel + 1, strlen(TheLabel));
	//printf("Clean Label ---- %s\n", cleanLabel);

	// Store data in TheValue register into TheLabel in memory.
	Attr * symEntryAttr = GetAttr(FindName(IdentSymTab, cleanLabel));
	//printf("TYPE TEST %d\n", symEntryAttr->TypeDesc->BaseType);
	if(symEntryAttr->Label == NULL){
		fprintf(stderr, "ERROR: Left side of assignment: %s --- not found in symbol table. Try declaring memory before assigning it.", cleanLabel);
		exit(-1);
	}

	//Append and free temporary registers
	
	newInstr = GenInstr(NULL, "sw", TmpRegName(TheValue->TheResultReg), TheLabel, NULL);
	newInstr = AppendSeq(TheValue->TheInstrSeq, newInstr);
	ReleaseTmpReg(TheValue->TheResultReg);
	//free(TheValue->TheInstrSeq);
	free(TheValue);

	return newInstr;
}

struct ExprResult * ProcGet( struct TypeDesc * TheType){
	ExprResult * TheResult = malloc (sizeof(ExprResult));
	TheResult->TheType = TheType;	
	InstrSeq * newInstr;
	//Get new register to store combined result
	TheResult->TheResultReg = AvailTmpReg();

	//Generate instructions to load the read syscall into special $v0 register. Then syscall to perform the read. It puts result in $v0.
	//Move $v0 into your available tmp register and then store that value into the memory location
	TheResult->TheInstrSeq = GenInstr(NULL, "li", "$v0", "5", NULL);
	AppendSeq(TheResult->TheInstrSeq, GenInstr(NULL, "syscall", NULL, NULL, NULL));
	AppendSeq(TheResult->TheInstrSeq, GenInstr(NULL, "move", TmpRegName(TheResult->TheResultReg), "$v0", NULL));

	return TheResult;
}

struct InstrSeq * ProcPutStmt( struct ExprResult * TheValue){	

	//generate syscall to print integer or char depending on type
	//printf("THE TYPE VALUE IN PUT STMT! ----------------------- %d\n", (int) TheValue->TheType->BaseType);
	if(TheValue->TheType->BaseType == 0){
		AppendSeq(TheValue->TheInstrSeq, GenInstr(NULL, "li", "$v0", "1" , NULL));
	}
	else{
		AppendSeq(TheValue->TheInstrSeq, GenInstr(NULL, "li", "$v0", "11" , NULL));
	}
		

	//In the special syscall register $v0, 1 will print an integer and 11 will print a char if given ascii/an int.
	AppendSeq(TheValue->TheInstrSeq, GenInstr(NULL, "move", "$a0", TmpRegName(TheValue->TheResultReg), NULL));	
	AppendSeq(TheValue->TheInstrSeq, GenInstr(NULL, "syscall", NULL, NULL, NULL));

	//printf("&&&&&&&&&&&&&&&&TheValue Op: %s   Reg1: %s    Reg2: %s\n", TheValue->TheInstrSeq->OpCode, TheValue->TheInstrSeq->Oprnd1, TheValue->TheInstrSeq->Oprnd2);
	ReleaseTmpReg(TheValue->TheResultReg);
	//This caused a major memory corruption issue later where sequences got overwritten with garbage and segfaulted when attempting to print/append.
	//free(TheValue->TheInstrSeq);

	//Save reference to instrSeq for return after we free old ExprResult.
	InstrSeq * newInstrSeq = TheValue->TheInstrSeq;
	free(TheValue);
	//printf("&&&&&&&&&&&&&&&NewInstr Op: %s   Reg1: %s    Reg2: %s\n", newInstrSeq->OpCode, newInstrSeq->Oprnd1, newInstrSeq->Oprnd2);
	return newInstrSeq;
}	


void
Finish(struct InstrSeq * DeclsCode, struct InstrSeq * BodyCode)
{
  printf("Finish\n");
  	
  //printInstrSeq(BodyCode);
  ListSymTab();
  struct InstrSeq  *dataSection, * finalCode, * exitCode;
 
  dataSection = GenInstr(NULL,".data",NULL,NULL,NULL);
  dataSection = AppendSeq(dataSection, DeclsCode);
  
  exitCode = AppendSeq(GenInstr(NULL,"li","$v0","10",NULL), GenInstr(NULL,"syscall",NULL,NULL,NULL));
  BodyCode = AppendSeq(BodyCode, exitCode);
  		
  finalCode = AppendSeq(AppendSeq(GenInstr(NULL,".text",NULL,NULL,NULL), BodyCode), dataSection);
  
  	
  WriteSeq(finalCode);
  
  // write code sequences out to the assembly file
  // this will boilerplate for text and data segment headers and alignment information
  
  // there should be only one call to WriteSeq with entire program since WriteSeq closes the
  // file handle when done
  
  // Choice: generate data segment statements for variables as they are declared, pass up through
  // DeclsCode and concatenate with remainder of code or enumerate symbol table now and generate
  // data segment statement for variables
  
}
