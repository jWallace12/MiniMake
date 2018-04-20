


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "arg_parse.h"


/* Word Counter
 * line The string to process
 * This function returns the count of items
 * in a string seperated by whitespace
*/
static int wordCounter(char* line) {
  int numWords = 0;
  bool onWord = false;
  char lastChar = '\0';
  for (int i = 0; line[i] != '\0'; i++) {
    lastChar = line[i];
      if (onWord) {
        if(isspace(line[i])) {
          numWords++;
	  onWord = false;
	  line[i] = '\0';
        }
      } else {
	if (!(isspace(line[i]))) {
	  onWord = true;
	} else {
	  line[i] = '\0';
	}
      }
  }
  if (!isspace(lastChar)) {
    numWords++;
  }
  return numWords;
}

/* Word Parse
 * line The entire string
 * len The count of non-whitespace characters
 * This function converts a char* to a char**
 * by returning an array of pointers, with each 
 * pointer pointing a full word in the original
 * char*
*/
static char** wordParse(char* line, int len) {
  char** args = malloc((len + 1) * (sizeof(char*)));
  args[len] = NULL;

  bool onWord = false;
  int lineIndex = 0;
  int argsIndex = 0;
  while (argsIndex != len) {
    if (onWord) {
	if (line[lineIndex] == '\0') {
	  onWord = false;
	}
    } else {
	if (line[lineIndex] != '\0') {
	  args[argsIndex] = &line[lineIndex];
	  argsIndex++;
	  onWord = true;
	}
    }
    lineIndex++;
  }
//  free(args);
  return args;
}
/*Arg Parse
 * line A string of commands
 * argcp Variable to save the number of char* 
 * indices in the resulting char**
 * This function converts a char* into a char**
 * It counts all words in line, then returns an
 * array of pointers with each point pointing to
 * the words in line
*/
char** arg_parse(char* line, int *argcp){

  int numWords = wordCounter(line);
  *argcp = numWords;
  return wordParse(line, numWords);

}

