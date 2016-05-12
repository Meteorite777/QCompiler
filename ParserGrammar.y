%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IOMngr.h"

extern int yylex();    /* The next token function. */
extern char *yytext;   /* The matched token text.  */
extern int yyleng;     /* The token text length.   */
void yyerror(char *s);
#define YYSTYPE long



%}

%token EOF_TOK     0
%token BEGIN_TOK   1  END_TOK    2
%token ASSIGN_TOK  3  SEMI_TOK   4   LPAREN_TOK  5   RPAREN_TOK  6
%token MINUS_TOK   7  PLUS_TOK   8   TIMES_TOK   9   DIV_TOK    10
%token INTLIT_TOK 11  IDENT_TOK 13

%%
Prog    : BEGIN_TOK StmtSeq END_TOK                     {               };
StmtSeq : Stmt StmtSeq                                  {               };
StmtSeq :                                               {               };
Stmt    : IDENT_TOK ASSIGN_TOK Expr SEMI_TOK            { printf("%s%s%s\n",(char*)$3, ":", "=");};
Expr	: Term MExpr					{ char* s=malloc(sizeof(char)*(
							strlen((char*)$1)+strlen((char*)$2)+1));
                                			strcpy(s,(char*)$1); strcat(s,(char*)$2);
                                			$$=(YYSTYPE)s;/*$$ = $1 + $2;*/	};
MExpr	: AddOp Term MExpr				{ char* s=malloc(sizeof(char)*(
							strlen((char*)$1)+strlen((char*)$2)+strlen((char*)$3)+1));
                                			strcpy(s,(char*)$2); strcat(s,(char*)$1); strcat(s,(char*)$3);
                                			$$=(YYSTYPE)s;/*printf("%s ", $1);/**/};
MExpr	: 						{ $$ = (YYSTYPE) "";	};
Term	: Factor MTerm					{ char* s=malloc(sizeof(char)*(
							strlen((char*)$1)+strlen((char*)$2)+1));
                                			strcpy(s,(char*)$1); strcat(s,(char*)$2);
                                			$$=(YYSTYPE)s;/*$$ = $1 + $2;*/	};
MTerm	: MultOp Factor MTerm				{ char* s=malloc(sizeof(char)*(
							strlen((char*)$1)+strlen((char*)$2)+strlen((char*)$3)+1));
                                			strcpy(s,(char*)$2); strcat(s,(char*)$1); strcat(s,(char*)$3);
                                			$$=(YYSTYPE)s;/*printf("%s ", $1);/*$$ = $2 + $1 + $3;*/};
MTerm	: 						{ $$ = (YYSTYPE) "";	};
Factor	: LPAREN_TOK Expr RPAREN_TOK			{ $$ = $2;/*printf("%s ", $2);/**/};
Factor	: MINUS_TOK Factor				{ char* neg=malloc(sizeof($2)+2); sprintf(neg, "-%s", (char*)$2); $$ = (YYSTYPE)neg;};
Factor	: INTLIT_TOK					{ $$ = $1;/*printf("%s ", $1); /**/};
Factor	: IDENT_TOK					{ $$ = $1;/* printf("%s ", $1); /**/};
AddOp	: MINUS_TOK					{ $$ = (YYSTYPE)"- ";	};
AddOp	: PLUS_TOK					{ $$ = (YYSTYPE)"+ ";	};
MultOp	: TIMES_TOK					{ $$ = (YYSTYPE)"* ";	};
MultOp	: DIV_TOK				        { $$ = (YYSTYPE)"/ ";	};


%%

void
yyerror(char *s)
{
  char msg[256];
  snprintf(msg,255,"err: \"%s\" yytext: \"%s\"\n",s,yytext);
  PostMessage(GetCurrentColumn(),msg);
}
