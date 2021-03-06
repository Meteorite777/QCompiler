Letter	[A-Za-z]
Digit   [0-9]
Integer	[1-9][0-9]*
Float   ((0|[1-9]*)\.[0-9]+|[0-9]+)
ChrLit  '([.]|\\.)'

%{
#include "IOMngr.h"
#include "QTokens.h"
typedef union {
  char * Text;
  struct IdList * IdList;
  struct TypeDesc * TypeDesc;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
} YYSTYPE;
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
main              { if(comStatus()){ return MAIN_TOK;} }
\:			{if(comStatus()){ return COLON_TOK;} }
\;			{if(comStatus()){ return SEMICOLON_TOK;} }
\{			{if(comStatus()){ return LBRACE_TOK;} }
\}			{if(comStatus()){ return RBRACE_TOK;} }
\,			{if(comStatus()){ return COMMA_TOK;} }
\=			{if(comStatus()){ return ASSIGN_TOK;} }
\(			{if(comStatus()){ return LPAREN_TOK;} }
\)			{if(comStatus()){ return RPAREN_TOK;} }
\-			{if(comStatus()){ return MINUS_TOK;} }
\+			{if(comStatus()){ return PLUS_TOK;}}
\*			{if(comStatus()){ return TIMES_TOK;}}
\/			{if(comStatus()){ return DIV_TOK;}}
get			{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " "); return GET_TOK;}}
put			{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " "); return PUT_TOK;}}
int			{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " ");return INT_TOK;}}
char			{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " ");return CHAR_TOK;}}
{Letter}({Letter}|{Digit})*	{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " ");return IDENT_TOK;}}
'([^'\\\n]|\\.)'	{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " ");return CHRLIT_TOK;}}
{Integer}		{if(comStatus()){yylval.Text = strdup(yytext); strcat((char*)yylval.Text, " ");return INTLIT_TOK;}}
[\n]			{ eolComment = 0; }
[ \t]                   { }
.			{ }
%%
