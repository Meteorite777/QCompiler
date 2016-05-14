/* Semantics.h
   The action and supporting routines for performing semantics processing.
*/

#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"
#include "CodeGen.h"

/* Declaration of semantic record data types used in grammar.y %union */
struct IdList {
  struct SymEntry * TheEntry;
  struct IdList * Next;
}typedef IdList;

enum BaseTypes { IntBaseType, ChrBaseType };

struct TypeDesc {
  enum BaseTypes BaseType;
}typedef TypeDesc;

/* Attribute Structure for symbol table entries */
struct Attr {
  struct TypeDesc * TypeDesc;
  char * Label;
  /* memory location */
}typedef Attr;

/* ExprResult Structure used for operand type checking and result/register storage */
struct ExprResult {
  struct InstrSeq * TheInstrSeq;
  struct TypeDesc * TheType;
  int TheResultReg;
}typedef ExprResult;

/* Supporting Routines */
void InitSemantics();
void ListSymTab();

/* Semantics Actions */
struct IdList *     ProcUndId(char * IdText);
struct IdList *     ChainUndId(struct IdList * TheList, char * IdText);
struct InstrSeq *   ProcDecl(struct IdList * TheList, struct TypeDesc * TheType);
struct TypeDesc *   ProcTypeDesc(enum BaseTypes TheBaseType);

struct ExprResult * ProcExpr(struct ExprResult * Operand1, char * Operator,struct ExprResult * Operand2);
struct ExprResult * ProcFactorIntLit(char * IntLit);
struct ExprResult * ProcFactorChrLit(char * ChrLit);
struct ExprResult * ProcFactorNeg(struct ExprResult * Factor);
struct ExprResult * ProcFactorId(char * Ident);
char * ProcLVal(char* Ident);
struct InstrSeq * ProcAssign(char* TheLabel, struct ExprResult * TheValue);
struct ExprResult * ProcGet( struct TypeDesc * TheType);
struct InstrSeq * ProcPutStmt( struct ExprResult * TheValue);	 

void                Finish(struct InstrSeq * DeclsCode, struct InstrSeq * BodyCode);
