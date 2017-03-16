#define EOF_TOK 0
#define IDENT_TOK 1
#define INT_TOK 2
#define CHAR_TOK 3
#define MAIN_TOK 4
#define LBRACE_TOK 5
#define RBRACE_TOK 6
#define SEMICOLON_TOK 7
#define COLON_TOK 8
#define COMMA_TOK 9
#define INTLIT_TOK 10
#define CHRLIT_TOK 11
#define ASSIGN_TOK 12
#define LPAREN_TOK 13
#define RPAREN_TOK 14
#define MINUS_TOK 15
#define PLUS_TOK 16
#define TIMES_TOK 17
#define DIV_TOK 18
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
  char * Text;
  struct IdList * IdList;
  struct TypeDesc * TypeDesc;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
