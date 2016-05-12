#include <stdbool.h>

#include "IOMngr.h"
#include "RDGrammar.h"
#include "RDTokens.h"

bool Prog()
// <Prog>		  :==	BEGIN <StmtSeq> END	
{
  ENTER;
  
  if (!Match(BEGIN_TOK)) return LEAVE_FAIL;
  if (!StmtSeq()) return LEAVE_FAIL;
  if (!Match(END_TOK)) return LEAVE_FAIL;
	
  return LEAVE_SUCC;
}

bool StmtSeq()
// <StmtSeq>  :==	<Stmt> <StmtSeq>	
// <StmtSeq>	:==				
{
  ENTER;
  switch(CurToken){
	case IDENT_TOK :
		if (!Stmt()) return LEAVE_FAIL;
		if (!StmtSeq()) return LEAVE_FAIL;
	default :
		return LEAVE_SUCC;
  }
  return LEAVE_SUCC;
}

bool Stmt()
// <Stmt>		:==	<Ident> := <Expr> ;	
{
  ENTER;
  if (!Match(IDENT_TOK)) return LEAVE_FAIL;
  if (!Match(ASSIGN_TOK)) return LEAVE_FAIL;
  if (!Expr()) return LEAVE_FAIL;
  if (!Match(SEMI_TOK)) return LEAVE_FAIL;
  return LEAVE_SUCC;
}

bool Expr()
// <Expr>		:==	<Term> <MExpr>		
{
  ENTER;
  if (!Term()) return LEAVE_FAIL;
  if (!MExpr()) return LEAVE_FAIL;
  return LEAVE_SUCC;
}

bool MExpr()
// <MExpr>		:==	<AddOp> <Term> <MExpr>	
// <MExpr>		:==				
{
  ENTER;
  switch(CurToken){
	case PLUS_TOK :
	case MINUS_TOK :
		if (!AddOp()) return LEAVE_FAIL;
		if (!Term()) return LEAVE_FAIL;
  		if (!MExpr()) return LEAVE_FAIL;
	default :
		return LEAVE_SUCC;
  }
  return LEAVE_SUCC;
}

bool Term()
//<Term>		:==	<Factor> <MTerm>	
{
  ENTER;
  if (!Factor()) return LEAVE_FAIL;
  if (!MTerm()) return LEAVE_FAIL;
  return LEAVE_SUCC;
}

bool MTerm()
//<MTerm>		:==	<MultOp> <Factor> <MTerm>
//<MTerm>		:==				
{
  ENTER;
  switch(CurToken){
	case TIMES_TOK :
	case DIV_TOK :
		if (!MultOp()) return LEAVE_FAIL;
		if (!Factor()) return LEAVE_FAIL;
  		if (!MTerm()) return LEAVE_FAIL;
		return LEAVE_SUCC;
	case PLUS_TOK :
	case MINUS_TOK :
	case SEMI_TOK :
	case RPAREN_TOK :
		return LEAVE_SUCC;
	default :
		return LEAVE_FAIL;
  }
}

bool Factor()
// <Factor>	:==	( <Expr> )		
// <Factor>	:==	- <Factor>		
// <Factor>	:==	<IntLit>		
// <Factor>	:==	<RealLit>		
// <Factor>	:==	<Ident>			
{
   ENTER;
   switch(CurToken){
	case LPAREN_TOK :
		if (!Match(LPAREN_TOK)) return LEAVE_FAIL;
		if (!Expr()) return LEAVE_FAIL;
  		if (!Match(RPAREN_TOK)) return LEAVE_FAIL;
		break;
	case MINUS_TOK :
		if (!Match(MINUS_TOK)) return LEAVE_FAIL;
		if (!Factor()) return LEAVE_FAIL;
		break;
	case INTLIT_TOK :
		if (!Match(INTLIT_TOK)) return LEAVE_FAIL;
		break;
	case REALLIT_TOK :
		if (!Match(REALLIT_TOK)) return LEAVE_FAIL;
		break;
	case IDENT_TOK :
		if (!Match(IDENT_TOK)) return LEAVE_FAIL;
		break;
	default:
		return LEAVE_FAIL;
    }
	return LEAVE_SUCC;
	
		
}

bool AddOp()
// <AddOp>		:==	-			
// <AddOp>		:==	+			
{
  ENTER;
  switch (CurToken) {
	case MINUS_TOK :
  		if( !Match(MINUS_TOK) ) return LEAVE_FAIL;
		break;
	case PLUS_TOK :
		if( !Match(PLUS_TOK) ) return LEAVE_FAIL;
		break;
	default :
		return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

bool MultOp()
// <MultOp>	:==	*			
// <MultOp>	:==	/			
{
  ENTER;
  switch (CurToken) {
	case TIMES_TOK :
  		if( !Match(TIMES_TOK) ) return LEAVE_FAIL;
		break;
	case DIV_TOK :
		if( !Match(DIV_TOK) ) return LEAVE_FAIL;
		break;
	default :
		return LEAVE_FAIL;
  }
  return LEAVE_SUCC;
}

