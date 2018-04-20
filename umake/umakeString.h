#ifndef _UMAKESTRING_
#define _UMAKESTRING_

struct string_st;
typedef struct string_st string;
typedef string* string_list;

/* New String
 * name - The name of the new string
 * returns a new string with the name set to the parameter name
*/
string* new_string(char* name);

/* Add String
 * s    - The list of all strings
 * name - The name of the new string
 * appends a new string to s
*/
void addString(string_list* s, char* name);

/* For Each String
 * list - The list of all strings
 * action - A function to send each string through
 * processes each string in list with the function action
*/
void for_each_string(string_list list, void(*action)(char*));

/* Get String Name
 * str - The corresponding string
 * returns the name of str
*/
char* getStringName(string* str);

/* Exec Dep Rules
 * list   - The list of dependencies
 * action - The function to send each dependency through
 * processes the list of depencdency, sending each dependency
 * through action
*/ 
void execDepRules(string_list list, void(*action)(char*));

/* Check Timestamps
 * name - the name of the target file
 * list - the list of dependencies
 * checks to see if any of the dependencies are newer than 
 * the intended target file
*/
int check_timestamps(char* name, string_list list);
#endif
