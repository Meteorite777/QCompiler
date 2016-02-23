/** Author: Nicholas Frank
	Date: 2/15/2015
	Summary: This is an IO Manager implementation for the construction of a basic compiler.
**/

/*Helper function that takes a string and an integer to prepend before printing the string.
  It formats and writes to the correct output stream (listing file vs stdout) the messages posted from PostMessage().*/
void PostLine(char* aLine, int lineNum);

#include "IOMngr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE *ioSrc;
FILE *ioLst;

char *ioBuffer;
char **messageArr;
int ioLineNum, ioBuffLen, ioBuffPos, messageNum;
bool messageFlag;

char GetSourceChar(){
	char rtnChar;
	int i;
	if(ioSrc == NULL){
		fprintf(stderr, "Error: You must open a source file before attempting to read a character.");
		exit(-1);
	}

	if(ioBuffer == NULL){
		//Malloc space for Line Buffer and read line into buffer.
		ioBuffer = (char*) malloc(sizeof(char) * MAXLINE);

		//fgets() returns null on error or EOF encountered and trying to read agin. 
		if (fgets(ioBuffer, (MAXLINE - 1), ioSrc) == NULL){
			// fgets() returning null and EOF encountered means we have processed all input. 
			if (feof(ioSrc) != 0){
				free(ioBuffer);
				ioBuffer = " ";
			}
			if (ferror(ioSrc) != 0){
				fprintf(stderr, "Error: Problem reading source file.");
				exit(-1);
			}			
		}

		//Initialize buffer variables ioBuffPos is line column and messageFlag tells if there are any tokens found.
		ioBuffPos = 0;
		messageFlag = false;
		ioLineNum++;
		//Malloc and create space for PostMessage array to hold messages associated with line columns.
		messageArr = (char**) malloc(sizeof(char*) * strlen(ioBuffer));
		for(i = 0; i < strlen(ioBuffer); i++){
			messageArr[i] = NULL;
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
	if(feof(ioSrc) != 0 && ioBuffPos == (strlen(ioBuffer)) ){

		return EOF;
	}
	
	ioBuffPos++;
	return rtnChar;
}

void PostMessage(int aColumn, const char * aMessage){
	if(messageNum < (MAXENTRIES - 1)){
		char *newMessage = (char*) malloc(sizeof(char) * (strlen(aMessage) + 1));
		strcpy(newMessage, aMessage);
		
		messageArr[aColumn] = newMessage;
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
		for(i = 0; i < strlen(ioBuffer); i++){
			if(messageArr[i] != NULL){
				fputc(msgChar, outputStream);
				msgChar++;
			}
			else{
				fputc(' ', outputStream);
			}
		}
		// Print a newline as either a gap between empty lines or a line after msgChar's are printed.
		// However we don't need a newline if there are no messages to be posted.
		if(messageFlag == true){
			fputc('\n', outputStream);
		}
	
		// Reset message lettering back to A 
		msgChar = 'A';
		
		//Loop through the line columns and print any messages associated with them in our array.
		for(i = 0; i < strlen(ioBuffer); i++){
			if(messageArr[i] != NULL){
				fprintf(outputStream, "%4c-%c %s\n", ' ', msgChar, messageArr[i]);
				msgChar++;
			}
		}
		// We don't need a newline after the last source line.
		if(feof(ioSrc) == 0){
			fputc('\n', outputStream);
		}
	}
	//Loop through the line columns and free any messages associated with them in our array.
	for(i = 0; i < strlen(ioBuffer); i++){
		free(messageArr[i]);
		messageArr[i] = NULL;
	}
	free(messageArr);
	messageArr = NULL;
	messageNum = 0;
}

bool OpenFiles(const char * aSourceName, const char * aListingName){
	
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
	if(ioSrc != NULL){
		if(feof(ioSrc) != 0){
			PostLine(ioBuffer, ioLineNum);
		}
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
	return ioBuffPos - 1;
}
