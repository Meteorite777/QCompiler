/* CodeGen.h
   Routines to support the generation of assembly code.
   
*/

#include <stdio.h>

struct InstrSeq {
  char *Label;
  char *OpCode;
  char *Oprnd1;
  char *Oprnd2;
  char *Oprnd3;
  struct InstrSeq *Next;
}typedef InstrSeq;

void               InitCodeGen(char *AFilename);

struct InstrSeq *  GenInstr(char *Label, char *OpCode,
                                  char *Oprnd1, char *Oprnd2, char *Oprnd3);
struct InstrSeq *  AppendSeq(struct InstrSeq *Seq1,
                                   struct InstrSeq *Seq2);
void               WriteSeq(struct InstrSeq *ASeq);

char *             GenLabel();

int                AvailTmpReg();
char *             TmpRegName(int RegNum);
void               ReleaseTmpReg(int ANum);
void               ResetAllTmpReg();
struct InstrSeq *  SaveSeq();
struct InstrSeq *  RestoreSeq();

char *             Imm(int Val);
char *             RegOff(int Offset, char * Reg);
