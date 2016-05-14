%{
#include "IOMngr.h"
#include "Scanner.h"
#include "Semantics.h"
#include <string.h>
void yyerror(char *s);
#define YYDEBUG 1
%}

/* Union structure of data that can be attached to non-terminals */ 
%union {
  char * Text;
  struct IdList * IdList;
  struct TypeDesc * TypeDesc;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
}

/* Type declaration for data attched to non-terminals. Allows */
/* $# notation to be correctly type cased for function arguments. */
/* %type <union-field-name> non-terminal */
%type <Text> Id
%type <IdList> IdList
%type <TypeDesc> Type
%type <InstrSeq> Decl
%type <InstrSeq> Decls
%type <InstrSeq> Body

%type <InstrSeq> Stmt
%type <InstrSeq> StmtSeq
%type <InstrSeq> AssignStmt
%type <Text> LVal
%type <ExprResult> Expr
%type <ExprResult> Term
%type <ExprResult> Factor
%type <Text> AddOp
%type <Text> MultOp
%type <Text> INTLIT_TOK
%type <Text> CHRLIT_TOK

/* List of token name and corresponding numbers */
/* This must agree with QTokens.h */
%token EOF_TOK 0 IDENT_TOK 1 INT_TOK 2 CHAR_TOK 3 MAIN_TOK 4
%token LBRACE_TOK 5 RBRACE_TOK 6 SEMICOLON_TOK 7 COLON_TOK 8 
%token COMMA_TOK 9 INTLIT_TOK 10 CHRLIT_TOK 11 ASSIGN_TOK 12 
%token LPAREN_TOK 13 RPAREN_TOK 14 MINUS_TOK 15 PLUS_TOK 16 
%token TIMES_TOK 17 DIV_TOK 18 GET_TOK 19 PUT_TOK 20

%%

Prog       : Decls Body                              { Finish($1,$2); } ;

// Declarations
Id         : IDENT_TOK                               { $$ = strdup(yytext); };
Decls      : Decl  Decls                             { $$ = AppendSeq($1, $2); };
Decls      :                                         { $$ = NULL;};
Decl       : Type IdList SEMICOLON_TOK               { $$ = ProcDecl($2,$1); };
IdList     : IdList COMMA_TOK Id                     { $$ = ChainUndId($1,$3); };
IdList     : Id                                      { $$ = ProcUndId($1); };
Type       : INT_TOK                                 { $$ = ProcTypeDesc(IntBaseType); };
Type       : CHAR_TOK                                { $$ = ProcTypeDesc(ChrBaseType); };

// Main Body and Statements
Body       : MAIN_TOK LBRACE_TOK StmtSeq RBRACE_TOK  { $$ = $3; };
StmtSeq    : Stmt SEMICOLON_TOK StmtSeq              { $$ = AppendSeq($1, $3);};
StmtSeq    :                                         { $$ = NULL; };
Stmt	   : PUT_TOK LPAREN_TOK Factor RPAREN_TOK    { $$ = ProcPutStmt($3); };
Stmt       : AssignStmt                              { $$ = $1; };
Stmt	   : Expr				     { $$ = $1->TheInstrSeq; free($1);};
AssignStmt : LVal ASSIGN_TOK Expr		     { $$ = ProcAssign($1, $3);};
LVal       : Id				     	     { $$ = ProcLVal($1); };
Expr	   : Term AddOp Expr			     { $$ = ProcExpr($1, $2, $3);};
Expr	   : Term         			     { $$ = $1; };
Term	   : Factor MultOp Term			     { $$ = ProcExpr($1, $2, $3);};
Term	   : Factor         			     { $$ = $1; };
Factor	: LPAREN_TOK Expr RPAREN_TOK		     { $$ = $2; };
Factor	: GET_TOK LPAREN_TOK Type RPAREN_TOK	     { $$ = ProcGet($3);  };
Factor	: MINUS_TOK Factor			     { $$ = ProcFactorNeg($2); };
Factor	: INTLIT_TOK			 	     { $$ = ProcFactorIntLit($1); };
Factor	: CHRLIT_TOK				     { $$ = ProcFactorChrLit($1); };
Factor  : Id					     { $$ = ProcFactorId($1); };
AddOp	: MINUS_TOK				     { $$ = "- ";	};
AddOp	: PLUS_TOK				     { $$ = "+ ";	};
MultOp	: TIMES_TOK				     { $$ = "* ";	};
MultOp	: DIV_TOK				     { $$ = "/ ";	};
		
%%

void
yyerror( char *s)
{
  char msg[MAXLINE];
  sprintf(msg,"ERROR \"%s\" token: \"%s\"",s,yytext);
  PostMessage(GetCurrentColumn(),msg);
}
