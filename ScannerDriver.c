/* ScannerDriver.c

*/
#include <stdio.h>
#include <string.h>

#include "Tokens.h"

#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"

/* symbol table entries have two attributes, first line of occurrence and occurrence count */
struct Attributes {
  int firstLine;
  int cnt;
}typedef Attributes;

/* used with qsort to sort list of symbol table entries */
int
entryCmp(const void * A, const void * B)
{
  /* A is pointer to element of array which contains a pointer to a struct SymEntry */
  const char * strA = GetName(*(struct SymEntry **)A);
  const char * strB = GetName(*(struct SymEntry **)B);
  return strcmp(strA,strB);
}

// The Lookahead chracter in the yylex() algorithm messes with GetCurrentColumn and posting messages.
// When lex knows it's a fixed length reserved word it stops scanning at the last charater and returns
// a token to post, using the correct column in the listing. However when it's a regex token (float, integer, ident),
// it must look ahead one token to decide to stop parsing. Doing so increments the current column and incorrectly 
// posts the message one column too far ahead. Check for reserved words and return yyleng-2 if it's not one.
int GetMessageColumn(int token){
	// If it's nt a reserved
	if(token == 3 || token == 4 || token == 5){
		return GetCurrentColumn() - (yyleng);
	}
	else{
		return GetCurrentColumn() - (yyleng - 1);
	}
}

int
main(int argc, char **argv)
{ int Token;
  int entryCheck = 0;
  int commentCnt = 0;
  char message[256];
  struct SymTab * table = NULL;
  struct SymEntry * tableEntry = NULL;
  
  bool ret = OpenFiles("ScannerSource", "ScannerListing");
  if (!ret) {
    printf("Could not open source and listing files\n");
    exit(1);
  }
  
  while ((Token = yylex()) != 0) {
    sprintf(message,"Token#=%d, Length=%d, Text=\"%s\"",Token,yyleng,yytext);
    PostMessage(GetMessageColumn(Token) ,message);
    switch(Token) {
      case INIT_TOK: {
        /* create a symbol table 
         */
        if (!table) table = CreateSymTab(20);
        break; }
      case IDENT_TOK: {
        /* place the identifier in the table (if it exists), if new then create and init
           attributes structure, if already in table then update attributes cnt field, in
           either case post an appropriate message
         */	
	if(table != NULL){
		if(!EnterName(table, yytext, &tableEntry)){
			entryCheck++;
			//printf("adding entry %d: '%s' to the table.\n", entryCheck, yytext);
			struct Attributes * newAttr = (struct Attributes*) malloc(sizeof(struct Attributes));
			newAttr->firstLine = GetCurrentLine();
			newAttr->cnt = 1;
			SetAttr(tableEntry, newAttr);
			free(newAttr);
		}
		else{
			//Found in table
			//printf("Found entry %d: '%s' in the table.\n", entryCheck, yytext);
			struct Attributes * foundAttr = GetAttr(tableEntry);	
			foundAttr->cnt++;
			SetAttr(tableEntry, foundAttr);
			//free(foundAttr);
		}
	}
       break; }
      /* cases for other token types
       */
      case DUMP_TOK: {
        /* get table statistics, alloc an array to hold entry pointers
         */
        struct Stats * stats = Statistics(table);
        struct SymEntry ** entries = malloc(stats->entryCnt * sizeof(struct SymEntry *));
        
        /* enumerate the table collecting entry pointers into the array
         */
        int i = 0;
        struct SymEntry * entry = FirstEntry(table);
        entries[i] = entry;
        while (entry) {
          i++;
          entry = NextEntry(table, entry);
          entries[i] = entry;
        }

        /* sort the entries
         */
        qsort(entries,stats->entryCnt,sizeof(struct SymEntry *),entryCmp);
        
        /* list the contents of the table in sorted order
         */
        printf("\nContents of Symbol Table\n");
        for (int i = 0; i < stats->entryCnt; i++) {
          printf("%3d %20s %5d %5d\n", i,
                 GetName(entries[i]),
                 ((struct Attributes *) GetAttr(entries[i]))->firstLine,
                 ((struct Attributes *) GetAttr(entries[i]))->cnt);
        }
        free(stats);
        break; }
	
	/* Cases added to assignment by Nick for testing and for comments
 	 */
	case INTEGER_TOK: {
		//printf("Integer Token '%s' encountered.\n", yytext);
		break;
	}
	case FLOAT_TOK: {
		//printf("Integer Token '%s' encountered.\n", yytext);
		break;
	}
	case ADD_TOK: {
		//printf("Addition Operator Token '+' encountered.\n");
		break;
	}
	case COMM_START_TOK: {
		//printf("Comment Start Token '/*' encountered.\n");
		break;
	}

	case COMM_END_TOK: {
		//printf("Comment End Token '*/' encountered.\n");
		break;
	}
	
	case COMM_EOL_TOK: {
		//printf("Comment To End-of-Line Token '//' encountered.\n");
		break;
	}	
    }
  }
  //printf("End of Program\n");
  CloseFiles();
}
