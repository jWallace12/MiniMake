#ifndef _TARGET_H_
#define _TARGET_H_

struct target_st;
typedef struct target_st target;
typedef target* target_list;

/* New Target 
 * name - Name of desired target
 * returns a new target with the name set to the parameter name
*/
target* new_target(char* name);

/* Find Target
 * name - Name of the target that is being searched for
 * returns the target with the name that matches the parameter name
*/
target* find_target(char* name);

/* Add Dependency Target 
 * tgt - The target to attach the new dependency to
 * dep - The name of the new dependency to add to tgt
 * adds a new dependency to the linked list dependencies in tgt
*/
void add_dependency_target(target* tgt, char* dep);

/* Add Rule Target
 * tgt  - The target to attach the new rule to
 * rule - The name of the new rule 
 * adds a new rule to the linked list rules in tgt
*/
void add_rule_target(target* tgt, char* rule);

/* For Each Rule
 * tgt    - The target that has the rules to act on
 * action - A function to send each rule through
 * processes each rule in tgt to the function action
*/
void for_each_rule(target* tgt, void(*action)(char*));

/* For Each Dependency
 * tgt    - The target that has the dependencies to act on
 * action - A function to send each dependencies through
 * processes each depencency in tgt to the function action
*/
void for_each_dependency(target* tgt, void(*action)(char*));

/* Dependency Time Stamps
 * name - The name of the target file
 * tgt  - The target which holds the dependencies
 * checks the timestamps of each dependency from tgt
*/
int dependency_time_stamps(char* name, target* tgt);
#endif
