/* IOMngrDriver.c

*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "IOMngr.h"

int
main(int argc, char **argv)
{ char *src, *lst;
  
	if (argc == 1) {
	  src = "IOMngrSource";
		lst = "IOMngrListing";
	}
  else if (argc == 2) {
    src = argv[1];
    lst = NULL;
  }
  else if (argc == 3) {
    src = argv[1];
    lst = argv[2];
  }
  else {
    fprintf(stderr,"usage: IOMngrDriver [SourceName [ListingName]]\n");
    exit(1);
  }
  
  if (OpenFiles(src,lst)) {
    int eofCnt = 0;
    while(eofCnt < 3) {
      char curChar;
      if ((curChar = GetSourceChar()) == EOF) {
        eofCnt++;
        PostMessage(GetCurrentColumn(),"EOF found");
        continue;
      }
      if (isupper(curChar)) {
        PostMessage(GetCurrentColumn(),"found uppercase");
      }
      if (strchr("(){}",curChar)) {
        PostMessage(GetCurrentColumn(),"found grouping symbol");
      }
      if (strchr(",;",curChar)) {
        PostMessage(GetCurrentColumn(),"found punctuation");
      }
      if (strchr("=*",curChar)) {
        PostMessage(GetCurrentColumn(),"found operator symbol");
      }
   }
    CloseFiles();
  }
  else {
    printf("Files could not be opened.\n");
  }

  exit(0);
}
