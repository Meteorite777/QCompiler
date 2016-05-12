Letter	[A-Za-z]
Digit   [0-9]
Integer	[1-9][0-9]*
Float   ((0|[1-9]*)\.[0-9]+|[0-9]+)

%{
#include "RDTokens.h"
#include "IOMngr.h"

#define YYSTYPE long   /* 64 bit so can hold pointer and int */
extern YYSTYPE yylval;

int eolComment;
int commentCnt;
#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
          result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }
int comStatus(){
	if(eolComment == 0 && commentCnt == 0){
		return 1;
	}
	else{
		return 0;
	}
}

int comCheck(int tokenType){
	if(comStatus){ return tokenType;}
}
%}

%%
\/\*		{ commentCnt++; }
\*\/		{ commentCnt--; }
\/\/		{ eolComment = 1;}
BEGIN              { if(comStatus()){ return BEGIN_TOK;} }
END                { if(comStatus()){ return END_TOK;} }
\:\=			{if(comStatus()){ return ASSIGN_TOK;} }
\;			{if(comStatus()){ return SEMI_TOK;} }
\(			{if(comStatus()){ return LPAREN_TOK;} }
\)			{if(comStatus()){ return RPAREN_TOK;} }
\-			{if(comStatus()){ return MINUS_TOK;} }
\+			{if(comStatus()){ return PLUS_TOK;}}
\*			{if(comStatus()){ return TIMES_TOK;}}
\/			{if(comStatus()){ return DIV_TOK;}}
{Letter}({Letter}|{Digit})*	{if(comStatus()){yylval = (YYSTYPE) strdup(yytext); strcat((char*)yylval, " ");/*printf("I matched an Identifier: %s\n", yytext);*/ return IDENT_TOK;}}
{Integer}			{ if(comStatus()){yylval = (YYSTYPE) strdup(yytext); strcat((char*)yylval, " ");return INTLIT_TOK;}}
[\n]			{ eolComment = 0; }
[ \t]                   { }
.			{ }
%%
