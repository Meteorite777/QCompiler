/* IOMngr.h
 
   Implements a simple interface for reading characters from
   the source file while producing a listing file. Functionality
   includes
 
   - opening source and listing files by name
   - closing source and listing files
   - getting a single character from the source file
   - post message to be included after a source line
   - get current line number
   - get current column number
   
   - PostMessage registers a message to be associated with the current line
   and indicated by a marker in the indicated column. The letters 'A' ... 'Z' 
   are used as the markers, consequently, a maximum of 26 message lines are
   allowed for each source line. Calls to PostMessage beyond this limit are
   siliently ignored.
   
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
                        
#define MAXLINE 1024
#define MAXENTRIES 26

/* OpenFiles      Opens the two files given by the filenames. aSourceName is 
                  required but aListingName may be NULL. If aListingName is 
                  NULL output will be directed to stdout (the system output 
                  stream). If the opens are successful this function returns 
                  true, otherwise it returns false.

   CloseFiles     Closes the two files.
*/
bool OpenFiles(const char * aSourceName,
               const char * aListingName);
void CloseFiles(); 

/* GetSourceChar  Returns the next character in the source file. This
                  function must also arrange for source file lines to be echoed 
                  to the listing file with a prepended line number. If 
                  aListingName was not null then all source lines are written 
                  to the listing. If aListingName was null then only lines that 
                  have posted messages are written to the listing. As a
                  consequence, source lines must be read in their entirety and 
                  buffered. It is only when the characters of a line have been 
                  exhausted, and a new line must be read, that the line is 
                  considered for printing to the listing file. Any tab 
                  characters ('\t')found in the input buffer are replaced by a
                  single ' '. The EOF character is returned if the source file 
                  is at the end of file.
*/
char GetSourceChar();

/* PostMessage    Registers a message to be associated with the current line
                  and indicated by a marker in the indicated column. The 
                  letters 'A' ... 'Z' are used as the markers, consequently, 
                  a maximum of 26 message lines are allowed for each source 
                  line. Calls to PostMessage beyond this limit are siliently 
                  ignored.
*/
void PostMessage(int aColumn, const char * aMessage);

/* GetCurrentLine Returns the current line number in the source file.

   GetCurrentColumn Returns the column number of the most recently returned 
                  character.
*/
int GetCurrentLine();
int GetCurrentColumn();
