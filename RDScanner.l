Letter	[A-Za-z]
Digit   [0-9]
Integer	[1-9][0-9]*
Float   ((0|[1-9]*)\.[0-9]+|[0-9]+)

%{
#include "RDTokens.h"
#include "IOMngr.h"
#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
          result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }

%}

%%
BEGIN              { return(BEGIN_TOK); }
END                { return(END_TOK); }
\:\=			{return(ASSIGN_TOK);}
\;			{return(SEMI_TOK);}
\(			{return(LPAREN_TOK);}
\)			{return(RPAREN_TOK);}
\-			{return(MINUS_TOK);}
\+			{return(PLUS_TOK);}
\*			{return(TIMES_TOK);}
\/			{return(DIV_TOK);}
{Letter}({Letter}|{Digit})*	{/*printf("I matched an Identifier: %s\n", yytext);*/ return(IDENT_TOK);}
{Integer}			{ return(INTLIT_TOK);	}
{Float}				{ return(REALLIT_TOK);	}
[\n]			{ }
[ \t]                   { }
.			{ }
%%
