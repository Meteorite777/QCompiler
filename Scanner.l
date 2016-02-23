Letter	[A-Za-z]
Digit   [0-9]
Integer	[1-9][0-9]*
Float   [+-]?((0|[1-9]*)\.[0-9]+|[0-9]+)

%{
#include "Tokens.h"
#include "IOMngr.h"
int eolComment;
int commentCnt;
#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
	}


%}

%%
\$INIT                          { commentCnt = 0; return(INIT_TOK); }
\$DUMP                          { return(DUMP_TOK); }
{Letter}({Letter}|{Digit})*	{ if(eolComment == 0 && commentCnt == 0){/*printf("I matched an Identifier: %s\n", yytext);*/ return(IDENT_TOK);}}
{Integer}			{ return(INTEGER_TOK);	}
{Float}				{ return(FLOAT_TOK);	}
\+				{ return(ADD_TOK);	}
\/\*				{ commentCnt++; return(COMM_START_TOK); }
\*\/				{ commentCnt--; return(COMM_END_TOK); }
\/\/				{ eolComment = 1; return(COMM_EOL_TOK); }
[\n]				{ eolComment = 0; }
[ \t]                         	{ }
.                               { /*printf("I didn't match: %s\n", yytext);*/ }
%%
