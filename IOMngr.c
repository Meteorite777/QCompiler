/** Author: Nicholas Frank
	Date: 2/15/2015
	Summary: This is an IO Manager implementation for the construction of a basic compiler.
**/

/*Helper function that takes a string and an integer to prepend before printing the string.
  It formats and writes to the correct output stream (listing file vs stdout) the messages posted from PostMessage().*/
void PostLine(char* aLine, int lineNum);

/* symbol table entries have two attributes, first line of occurrence and occurrence count */
struct Attributes {
  int col;
  char* msg;
}typedef Attributes;

#include "IOMngr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

FILE *ioSrc;
FILE *ioLst;

char *ioBuffer, rtnChar;
struct Attributes **messageArr;
int i, ioLineNum, ioColNum, ioBuffLen, ioBuffPos, messageNum, currMsgNum;
bool messageFlag;

char GetSourceChar(){
	rtnChar = '\0';
	if(ioSrc == NULL){
		fprintf(stderr, "Error: You must open a source file before attempting to read a character.");
		exit(-1);
	}

	//If there is nothing in the buffer we need to grab a new line from the source file.
	if(ioBuffer == NULL){
		//Malloc space for Line Buffer and read line into buffer.
		ioBuffer = (char*) malloc(sizeof(char) * MAXLINE);

		//This is to detect EOF. If detected it clears ioBuffer and 
		//fgets() returns null on error or EOF encountered and trying to read agin. 
		if (fgets(ioBuffer, (MAXLINE - 1), ioSrc) == NULL){
			// feof returns non-zero when EOF has been encountered.
			// fgets() returning null and EOF encountered means we have processed all input. 
			if (feof(ioSrc) != 0){
				free(ioBuffer);
				ioBuffer = " ";
			}
			// This is to catch the other case if it was an error and not EOF.
			if (ferror(ioSrc) != 0){
				fprintf(stderr, "Error: Problem reading source file.");
				exit(-1);
			}			
		}
	
	
		//Initialize buffer variables ioBuffPos is line column and messageFlag tells if there are any tokens found.
		ioBuffPos = 0;
		ioLineNum++;
		messageFlag = false;
		
	}
	// If there was something in the buffer we want to increment the buffer position (We avoid doing this when the buffer
	// is first created above, it is initialized at 0 and not incremented by this. We also don't want to increment after we have
	// seen EOF because our EOF checking below relies on strlen of the buffer. It's set to " " above as a flag, and relies on strlen.
	else{
		// If there is something in the buffer and we haven't hit EOF increment the position.
		if(feof(ioSrc) == 0){
			ioBuffPos++;
		}
	}
		
	//If the current character is a tab replace it with a space.
	if(ioBuffer[ioBuffPos] == '\t'){
		ioBuffer[ioBuffPos] = ' ';
	}
	rtnChar = ioBuffer[ioBuffPos];
	
	//End of Line so we free our line buffer
	if(rtnChar == '\n'){
		//Call to PostLine to handle outputting the line
		PostLine(ioBuffer, ioLineNum);
		
		//printf("IOMngr.c: ioBuffer - %s\n", ioBuffer);
		free(ioBuffer);
		ioBuffer = NULL;
	}
	
	//END OF FILE read into buffer and on last character
	if(feof(ioSrc) != 0 && ioBuffPos == (strlen(ioBuffer) - 1) ){
		//PostLine(ioBuffer, ioLineNum);
		return EOF;
	}
	

	return rtnChar;
}

void PostMessage(int aColumn, const char * aMessage){
	if(messageNum < (MAXENTRIES - 1)){
		char *newMessage = (char*) malloc(sizeof(char) * (strlen(aMessage) + 1));
		strcpy(newMessage, aMessage);
		struct Attributes *newAttr = (struct Attributes*) malloc(sizeof(struct Attributes));
		newAttr->col = aColumn;
		newAttr->msg = newMessage;
		messageArr[messageNum] = newAttr;
		messageFlag = true;
		messageNum++;
	}
}

void PostLine(char *aLine, int lineNum){
	int i;
	char msgChar = 'A';
	FILE *outputStream;
	
	if(ioLst == NULL){
		outputStream = stdout;
	}
	else{
		outputStream = ioLst;
	}
	//If we have messages for this src line then print or if we have no listing file then we print all lines.
	if(messageFlag == true || ioLst != NULL){
		// Unless on EOF line, when printing src lines they should contain a newline already so we don't need to print one.
		// However when printing the last src line, we must enter a new line between the src line and the message letters.
		if(feof(ioSrc) == 0){
		
			fprintf(outputStream, "%5d: %s%7c", lineNum, aLine, ' ');
		
		}
		else{
			fprintf(outputStream, "%5d: %s\n%7c", lineNum, aLine, ' ');
		}
		// After printing the line, check for messages and print characters if any exist.
		
		ioColNum = 0;
		currMsgNum = 0;
		if(messageFlag == true){
			while(currMsgNum != messageNum){
				if(messageArr[currMsgNum] != NULL){
					if(currMsgNum == 0){
						if(messageArr[currMsgNum]->col == ioColNum){
							fputc(msgChar, outputStream);
							currMsgNum++;
							msgChar++;		
						}
						else{
							fputc(' ', outputStream);
							//ioColNum++;
						}
						ioColNum++;
					}
					else{
						// While not on first one, check if we are second message on same spot. If so ignore.
						//If this message is not in same place as last one
						if(messageArr[currMsgNum]->col != messageArr[currMsgNum-1]->col){
							//if we are on the right place/column position in the line post a letter
							if(messageArr[currMsgNum]->col == ioColNum){
								fputc(msgChar, outputStream);
								currMsgNum++;
								msgChar++;		
							}
							else{
								fputc(' ', outputStream);
								//ioColNum++;
							}
							ioColNum++;
						}
						else{
							currMsgNum++;
						}
					}
				}
				// There was a problem where it infinite loops when multiple messages are posted to the same line.
				// because ioColNum gets higher than the column in the message and increments forever. Fixed above.
				/*if(messageArr[currMsgNum]->col == ioColNum){
					fputc(msgChar, outputStream);
					currMsgNum++;
					msgChar++;		
				}
				else{
					fputc(' ', outputStream);
					ioColNum++;
				}
				//ioColNum++;*/
			}
			// Print a newline as either a gap between empty lines or a line after msgChar's are printed.
			// However we don't need a newline if there are no messages to be posted.
			fputc('\n', outputStream);
		}
	
		// Reset message lettering back to A 
		msgChar = 'A';
		
		//Loop through the line columns and print any messages associated with them in our array.
		for(i = 0; i < MAXENTRIES; i++){
			if(messageArr[i] != NULL){
				if(i == 0){
					fprintf(outputStream, "%4c-%c %s\n", ' ', msgChar, messageArr[i]->msg);
					msgChar++;
				}
				else{
					if(messageArr[i]->col == messageArr[i-1]->col){
						fprintf(outputStream, "%4c-%c %s\n", ' ', msgChar-1, messageArr[i]->msg);
					}
					else{
						fprintf(outputStream, "%4c-%c %s\n", ' ', msgChar, messageArr[i]->msg);
						msgChar++;
					}
				}
			}
		}
		// We don't need a newline after the last source line.
		if(feof(ioSrc) == 0){
			fputc('\n', outputStream);
		}
	}
	//Loop through the line columns and free any messages associated with them in our array.
	for(i = 0; i < messageNum; i++){
		free(messageArr[i]->msg);
		free(messageArr[i]);
		messageArr[i] = NULL;
	}
	messageNum = 0;
}

bool OpenFiles(const char * aSourceName, const char * aListingName){
	//Malloc and create space for PostMessage array to hold messages associated with line columns.
	messageArr = (struct Attributes**) malloc(sizeof(struct Attributes*) * MAXENTRIES);
	for(i = 0; i < MAXENTRIES; i++){
		messageArr[i] = NULL;
	}

	if (aSourceName == NULL){
		printf("You must provide atleast a source file argument.\n");
		return false;
	}
	
	ioSrc = fopen(aSourceName,"r"); 
	if (!ioSrc) {
		fprintf(stdout,"Error: Cannot open source file.\n");
		return false;
	}
	
	if(aListingName != NULL){
		ioLst = fopen(aListingName,"w+");
		if (!ioLst) {
			fprintf(stdout,"Error: Cannot open listing file.\n");
			return false;
		}
	}
	
	return true;
}

void CloseFiles(){
	PostLine(ioBuffer, ioLineNum);
	free(messageArr);
	messageArr = NULL;

	if(ioSrc != NULL){
		fclose(ioSrc);
		ioSrc = NULL;
	}
	if(ioLst != NULL){
		fclose(ioLst);
		ioLst = NULL;
	}
	
}

int GetCurrentLine(){
	return ioLineNum;
}

int GetCurrentColumn(){
	//const char matchStr[] = "[]{}()=*;,.\'\"\r\n\0";
	//printf("return char: %c\n", rtnChar);
	//if(strchr(matchStr, rtnChar)){
		//printf("strchr matched a discarded character so decrement position.\n");
		//return ioBuffPos - 1;	
	//}

	return ioBuffPos;
}
