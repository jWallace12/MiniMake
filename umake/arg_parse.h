#ifndef _ARG_PARSE_H_
#define _ARG_PARSE_H_

/* Arg_Parse
 * line  - the string to parse
 * argcp - the corresponding amount of arguments created
 * returns an array of strings from line
*/
char** arg_parse(char *line, int *argcp);

#endif

