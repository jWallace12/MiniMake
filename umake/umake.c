/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 * Edited by Jonah Wallace - Winter 2018
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include "arg_parse.h"
#include "target.h"
#include "umakeString.h"
#include <fcntl.h>


/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void processline(char* line);

/* Assign Var
 * line	  The string containing the environment variable assignment
 * This function assigns an environment variable from a given string
 */
void assignVar(char* line);

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An ouput buffer that will contain a copy of orig with all
 *         variables expanded
 * newsize The size of the buffer point to by new
 * returns 1 upon success or 0 upon failure.
 * 
 * Example: "Hello, ${PLACE}" will expand to "Hello, World" when the environment
 * variable PLACE="World".
 */
int expand(char* orig, char* new, int newsize);

/* Process Target
 * line The name of the target with its dependencies
 * This function creates a new target and links its dependencies to itself
 * It then assigns the target as the global target
*/
target* processTarget(char* line);

/* Process Rules
 * line The rule for the global target
 * This function links the rule to the global target
*/
void processRules(target* target, char* line);

/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The command-line argument valus
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time.  Lines are processed
 * as targets with their dependencies list, and links said dependencies
 * to the target. Lines leading with the character ('\t') are interpreted
 * as a rule and linked to the target listed directly before the rule
 * minus the lading tab. After the file is processed, umake finds each
 * listed target from the command-line arguments and sends each of its
 * rules to processline.
 */
int main(int argc, const char* argv[]) {
  FILE* makefile = fopen("./uMakefile", "r");
  if (makefile == NULL) {
    fprintf(stderr, "%s\n", "No uMakefile found. Exitting...");
    exit(1);
  }
  size_t  bufsize = 0;
  char*   line    = NULL;
  ssize_t linelen = getline(&line, &bufsize, makefile);
  target* currTarget = NULL;
  while(-1 != linelen) {
    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    if(line[0] == '\t') {
      processRules(currTarget, &line[1]);
    } else {
      if (strchr(line, '=')) {
	assignVar(line);
      } else if (strchr(line, ':')) {
      	currTarget = processTarget(&line[0]);
      }
    }

    linelen = getline(&line, &bufsize, makefile);
  }
  for(int i = 1; argv[i] != '\0'; i++) {
    target* foundTarget = find_target((char*)argv[i]);
    
    if (foundTarget != NULL) {
      	for_each_dependency(foundTarget, processline);
	FILE* currFile = fopen((char*)argv[i], "r");
	int timestamps = dependency_time_stamps((char*)argv[i], foundTarget);
	if ((currFile == NULL) || (timestamps == 1)) {
      		for_each_rule(foundTarget, processline);
	}
    }
  }
  free(line);
  return EXIT_SUCCESS;
}

/* Process Targets */
target* processTarget(char* line) {
  int i = 0;
  while(line[i] != ':') {
    i++;
  }
  line[i] = ' ';
  int numArgs = 0;
  char** parsed_args = arg_parse(line, &numArgs);
  if (numArgs == 0) {
    free(parsed_args);
    return NULL;
  }
  char* newTarget = parsed_args[0];
  target* target = new_target(newTarget);
  int index = 1;
  for (; parsed_args[index] != '\0'; index++) {
    add_dependency_target(target, parsed_args[index]);
  }
  free(parsed_args);
  return target;
}

/* Process Rules */
void processRules(target* target, char* line) {
  add_rule_target(target, line);
}

/* Assign Var */
void assignVar(char* line) {
  int i = 0;
  while(line[i] != '=') {
    i++;
  }
  line[i] = ' ';
  for (int j = 0; j < strlen(line); j++) {
    if (line[j] == '#') {
      line[j] = '\0';
    }
  }
  int splitLen = 0;
  char** split = arg_parse(line, &splitLen);
  int status = setenv(split[0], split[1], 1);
  if (status == -1) {
    printf("Setting environment variable failed... Moving on\n");
  }
  free(split);
}

/* Expand */
int expand(char* orig, char* new, int newsize) {
  int newIndex = 0;
  int varIndex = 0;
  for (int i = 0; i < strlen(orig); i++) {
    if (orig[i] == '#') {
	break;
    }
    if ((i != 0) && (orig[i] == '>') && (orig[i-1] != '>')) {
      new[newIndex] = ' ';
      newIndex++;
      if (newIndex == newsize) {
        return 0;
      }
    }
    if ((i != 0) && (orig[i] != '>') && (orig[i-1] == '>')) {
      new[newIndex] = ' ';
      newIndex++;
      if (newIndex == newsize) {
  	return 0;
      }
    }


    if ((orig[i] == '$') && (i < strlen(orig)-1) && (orig[i+1] == '{')) {
      int varLen = 0;
      varIndex = i;
      i += 2;
      while (orig[i] != '}') {
	if ((i == strlen(orig)) || (isspace(orig[i]))) {
	  return 0;
	}
        varLen++;
	i++;
	if (varLen > strlen(orig)) {
	  return 0;
	}
      }
      if (varLen == 0) {
	return 0;
      }
      char* envVar = strndup(&orig[varIndex+2], varLen);
      char* expandedVar = getenv(envVar);
      free(envVar);
      if (expandedVar != NULL) {
        for (int j = 0; j < strlen(expandedVar); j++) {
	  new[newIndex] = expandedVar[j];
	  newIndex++;
	  if (newIndex == newsize) {
	    printf("Buffer is too full\n");  
	    return 0;
	  }
        }
      }
    } else {
      new[newIndex] = orig[i];
      newIndex++;
      if (newIndex == newsize) {
	return 0;
      }	
    }
  }
  new[newIndex] = '\0';
  return 1;
}


/* Process Line */
void processline (char* line) {
  char newBuf[1024];
  int status = expand(line, newBuf, 1024);
  if (status != 1) {
    printf("Failed to parse line. Processing next line...\n");
    return;
  }
  int numArgs = 0;
  char* buff_copy = strdup(newBuf);
  char** parsed_args = arg_parse(buff_copy, &numArgs);  

  if (numArgs == 0) {
    free(buff_copy);
    free(parsed_args);
    return;
  }
  int redirectIndex = 0;
  int appendIndex = 0;
  int truncateIndex = 0;
  for (int i = 0; parsed_args[i] != NULL; i++) {
    char* curr = parsed_args[i];
    if ((strlen(curr) == 2) && (curr[0] == '>') && (curr[1] == '>')) {
      appendIndex = i;
    } else if (curr[0] == '>') {
      truncateIndex = i;
    } else if (curr[0] == '<') {
      redirectIndex = i;
    }
  } 
  const pid_t cpid = fork();
  switch(cpid) {
 
  case -1: {
    perror("fork");
    break;
  }
 
  case 0: {
    if ((truncateIndex > 0) && (redirectIndex > 0)) { 
      close(0);
      open(parsed_args[redirectIndex+1], O_RDONLY|O_CREAT);
      close(1);
      open(parsed_args[truncateIndex+1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
      parsed_args[redirectIndex] = NULL;
      execvp(parsed_args[0], parsed_args);
      exit(EXIT_FAILURE);
    } else if ((appendIndex > 0) && (parsed_args[appendIndex+1] != NULL)) { 
      close(1);
      open(parsed_args[appendIndex+1], O_WRONLY|O_CREAT|O_APPEND, 0666);
      parsed_args[appendIndex] = NULL;
      execvp(parsed_args[appendIndex-1], parsed_args);
      exit(EXIT_FAILURE);
    } else if (truncateIndex > 0) {
      close(1);
      open(parsed_args[truncateIndex+1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
      parsed_args[truncateIndex] = NULL;
      execvp(parsed_args[0], parsed_args);
      exit(EXIT_FAILURE);
    } else if (redirectIndex > 0) {
      close(0);
      open(parsed_args[redirectIndex+1], O_RDONLY|O_CREAT);
      parsed_args[redirectIndex] = NULL;
      execvp(parsed_args[0], parsed_args);	
    } else {
      execvp(parsed_args[0], parsed_args);
      perror("execlp");
      exit(EXIT_FAILURE);
    }
    break;
  }

  default: {
    int   status;
    const pid_t pid = wait(&status);
    if(-1 == pid) {
      perror("wait");
    }
    else if (pid != cpid) {
      fprintf(stderr, "wait: expected process %d, but waited for process %d",
              cpid, pid);
    }
    break;
  }
  }
  free(buff_copy);
  free(parsed_args);
}
