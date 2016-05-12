/* Recursive Descent Support Functions */
#include <string.h>
#include <stdbool.h>

#include "RDSupport.h"
#include "Scanner.h"
#include "IOMngr.h"

int CurToken;
int Failed = 0;

char * callTrail = NULL;

void
pushTrail(const char * name)
{
  if (callTrail) {
    char * newTrail = malloc(strlen(callTrail) + strlen(name) + 2 + 1);
    sprintf(newTrail,"%s> %s",callTrail,name);
    free(callTrail);
    callTrail = newTrail;
  }
  else {
    callTrail = strdup(name);
  }
}

void
popTrail()
{
  char * p = strrchr(callTrail,'>');
  if (p) *p = '\0';
}

void
Enter(const char *Name)
{
  pushTrail(Name);
  printf("enter %s\n",callTrail);
}

bool
LeaveFail()
{
  popTrail();
  printf("leave fail %s\n",callTrail);
  return false;
}

bool
LeaveSucc()
{
  popTrail();
  printf("leave succ %s\n",callTrail);
  return true;
}

void 
ParseError(char *Message)
{
  char * buf = malloc(strlen(callTrail) + strlen(Message) + 34 + 1);
	sprintf(buf,"CurToken: %d -- %s -- %s\n",CurToken,callTrail,Message);
	PostMessage(GetCurrentColumn(),buf);
  free(buf);
}

bool 
Match(int ExpectedToken)
{
  if (CurToken == ExpectedToken) {
    CurToken = yylex();
    return true;
  }
  else {
    if (!Failed) ParseError("Token Match Failed");
		Failed = true;
    return false;
  }
}

