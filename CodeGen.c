#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "CodeGen.h"

char *AssmFilename;
FILE *AssmFile;

int NextLabel;
int counter;
struct TmpReg {
  unsigned char Free;
  unsigned char Used;
  char * Name;
} Registers[10] 
  = { {1, 0, "$t0"},  {1, 0, "$t1"},  {1, 0, "$t2"},  {1, 0, "$t3"},  
      {1, 0, "$t4"},  {1, 0, "$t5"},  {1, 0, "$t6"},  {1, 0, "$t7"},  
      {1, 0, "$t8"},  {1, 0, "$t9"}};
#define MAXREG 10

char Buf[16];

void
InitCodeGen(char *AFilename)
{
  AssmFilename = malloc(strlen(AFilename)+1);
  strcpy(AssmFilename,AFilename);
  AssmFile = fopen(AssmFilename,"w");

  NextLabel = 1;
  
}

char *
CopyStr(char * AStr)
{
  return (AStr) ? strdup(AStr) : NULL;
}

struct InstrSeq *     
GenInstr(char *Label, char *OpCode, char *Oprnd1, char *Oprnd2, char *Oprnd3)
{ struct InstrSeq *instr;
  
  instr = malloc(sizeof(struct InstrSeq));
  instr->Label = CopyStr(Label);
  instr->OpCode = CopyStr(OpCode);
  instr->Oprnd1 = CopyStr(Oprnd1);
  instr->Oprnd2 = CopyStr(Oprnd2);
  instr->Oprnd3 = CopyStr(Oprnd3);
  instr-> Next = NULL;
  

   /*Print Instruction Seqence information 
   if (instr->Label)printf("NEW CODEGEN INSTR -- %s\t",instr->Label); else printf("NEW CODEGEN INSTR -- (null)\t");
   if (instr->OpCode) printf("%s\t",instr->OpCode);else printf("(null)\t");
	fflush(stdout);
   if (instr->Oprnd1) printf("%s\t",instr->Oprnd1);else printf("(null)\t");
	fflush(stdout);
   if (instr->Oprnd2) printf("%s\t",instr->Oprnd2);else printf("(null)\t");
	fflush(stdout);
   if (instr->Oprnd3) printf("%s\n",instr->Oprnd3);else printf("(null)\n"); */
   
  return instr;
}

void printInstrSeq( struct InstrSeq * instr){
//Print all Instr's
  if(instr != NULL){
   if (instr->Label ) printf("INSTR -- %s\t",instr->Label);else printf("INSTR -- (null)\t");
   if (instr->OpCode) printf("%s\t",instr->OpCode);else printf("(null)\t");
   if (instr->Oprnd1) printf("%s\t",instr->Oprnd1);else printf("(null)\t");
   if (instr->Oprnd2) printf("%s\t",instr->Oprnd2);else printf("(null)\t");
   if (instr->Oprnd3) printf("%s\n",instr->Oprnd3);else printf("(null)\n");
   /*if (instr->Label ) printf("INSTRLEN -- %d\t",(int)strlen(instr->Label));else printf("INSTR -- (null)\t");
   if (instr->OpCode) printf("%d\t",(int)strlen(instr->OpCode));else printf("(null)\t");
   if (instr->Oprnd1) printf("%d\t",(int)strlen(instr->Oprnd1));else printf("(null)\t");
   if (instr->Oprnd2) printf("%d\t",(int)strlen(instr->Oprnd2));else printf("(null)\t");
   if (instr->Oprnd3) printf("%d\n",(int)strlen(instr->Oprnd3));else printf("(null)\n");*/
 }
} 

extern struct InstrSeq * 
AppendSeq(struct InstrSeq *Seq1, struct InstrSeq *Seq2)
{ struct InstrSeq *aInstr;

  //printf(">>>>>>Entering AppendSeq call: %d\n", counter++);
  //if(Seq1) printInstrSeq(Seq1); else printf("SEQ 1 WAS NULL in AppendSeq!####################\n");
  //if(Seq2) printInstrSeq(Seq2); else printf("SEQ 2 WAS NULL in AppendSeq!####################\n");

  if (!Seq1){ 
	//printf("<<<<<<<<<Returning Seq2 cause Seq1 was null.\n");

	return Seq2;
  }
  aInstr = Seq1;
  //printf("################## Begin Printing Whole Sequence\n");
  while (aInstr->Next){
	//printInstrSeq(aInstr);	
	aInstr = aInstr->Next;
  }
  //printInstrSeq(aInstr);
  aInstr->Next = Seq2;
  while (aInstr->Next){
	//fprintInstrSeq(aInstr->Next);	
	aInstr = aInstr->Next;
  }

  //printInstrSeq(Seq1);
  //if(!Seq2) printInstrSeq(Seq2);
  //printf("<<<<<<<<<Leaving AppendSeq call: %d returning %s\n", counter, "Seq1");
  return Seq1;
}

void    
WriteSeq(struct InstrSeq *ASeq)
{ struct InstrSeq *instr;

  instr = ASeq;
  while (instr) {
    if (instr->Label) fprintf(AssmFile,"%s:",instr->Label);
    if (instr->OpCode) {
      fprintf(AssmFile,"\t%s\t",instr->OpCode);
      if (instr->Oprnd1) fprintf(AssmFile,"\t%s",instr->Oprnd1);
      if (instr->Oprnd2) fprintf(AssmFile,", %s",instr->Oprnd2);
      if (instr->Oprnd3) fprintf(AssmFile,", %s",instr->Oprnd3);
    }
    fprintf(AssmFile,"\n");
    instr = instr->Next;
  }
  fclose(AssmFile);
}

char *
GenLabel()
{ char *label;

  label = (char *) malloc(8);
  sprintf(label,"L%d",NextLabel++);
  
  return label;
}
   
int
AvailTmpReg()
{  int i;

  for (i = 0; i < MAXREG; i++) {
    if (Registers[i].Free) {
      Registers[i].Free = 0;
      Registers[i].Used = 1;
      return i;
    }
  }
  
  return -1;
}

char *
TmpRegName(int RegNum)
{
  if ((RegNum >= 0) && ( RegNum < MAXREG)) {
    return Registers[RegNum].Name;
  } 
  else {
    return NULL;
  }
}

void
ReleaseTmpReg(int ANum)
{
  if ((ANum >= 0) && ( ANum < MAXREG)) {
    Registers[ANum].Free = 1;
  } 
  return;
}

void
ResetAllTmpReg()
{ int i;

  for (i = 0; i < MAXREG; i++) {
    Registers[i].Free = 1;
    Registers[i].Used = 0;
  }

  return;
}

struct InstrSeq * 
SaveSeq()
{ struct InstrSeq * save, *code;
  int i, scnt;
  char addr[8], offset[8];
  
  scnt = 0;
  save = NULL;
  for (i = 0; i < MAXREG; i++) {
    if (!Registers[i].Free) {
      scnt++;
      sprintf(addr,"%d($sp)",scnt*4);
      save = AppendSeq(save,GenInstr(NULL,"sw",TmpRegName(i),addr,NULL));
    }
  }
  if (scnt > 0) {
    sprintf(offset,"%d",scnt*4);
    code = GenInstr(NULL,"subu","$sp","$sp",offset);
    AppendSeq(code,save);
  }
  
  return code;
}

struct InstrSeq * 
RestoreSeq()
{ struct InstrSeq * code, * save;
  int i, scnt;

  char addr[8], offset[8];
  
  scnt = 0;
  save = NULL;
  for (i = 0; i < MAXREG; i++) {
    if (!Registers[i].Free) {
      scnt++;
      sprintf(addr,"%d($sp)",scnt*4);
      save = AppendSeq(save,GenInstr(NULL,"lw",TmpRegName(i),addr,NULL));
    }
  }
  if (scnt > 0) {
    sprintf(offset,"%d",scnt*4);
    code = AppendSeq(save,GenInstr(NULL,"addu","$sp","$sp",offset));
  }
  
  return code;
}

char *             
Imm(int Val)
{
  sprintf(Buf,"%d",Val);
  return Buf;
}

char *             
RegOff(int Offset, char * Reg)
{
  sprintf(Buf,"%d(%s)",Offset,Reg);
  return Buf;
}


