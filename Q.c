/* Main of Q Language Compiler */

#include <strings.h>
#include <string.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "Grammar.h"
#include "Semantics.h"

int
main(int argc, char * argv[])
{ char SourceName[MAXLINE], ListingName[MAXLINE], AssemblyName[MAXLINE];
  
  if (argc == 2) {
    strcpy(SourceName,argv[1]);
    strcpy(ListingName,argv[1]);
    strcpy(AssemblyName,argv[1]);
    strcat(SourceName,".src");
    strcat(ListingName,".lst");
    strcat(AssemblyName,".asm");
    if (!OpenFiles(SourceName,ListingName)) {
       printf("Source File Not Found.\n");
       exit(1);
    }
    InitSemantics();
    InitCodeGen(AssemblyName);
    if (yyparse() == 0) {
      PostMessage(GetCurrentColumn(),"Parse Successful");
    }
    else {
      PostMessage(GetCurrentColumn(),"Parse Unsuccessful");
    }
    CloseFiles();
  }
  else {
    printf("Usage: Q base-filename\n");
  }
  
  exit(0);
}
