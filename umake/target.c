#include "target.h"
#include "umakeString.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
	
	
struct target_st {
  target* next;
  char* name;
  string* dep;
  string* rule;
};

typedef target* target_list;
static target_list theTargets;

/* Append Target */
void appendTarget(target_list* list, target* t) {
  while(*list != NULL) {
    list = &((*list)->next);
  }
  *list = t;
}

/* New Target */
target* new_target(char* name) {
  target* t = malloc(sizeof(target));
  if (t != NULL) {
    t->next = NULL;
    t->name = strdup(name);
    t->dep = NULL;
    t->rule = NULL;
  }
  appendTarget(&theTargets, t);
  return t;
}

/* Find Target */
target* find_target(char* name) {
  target_list l = theTargets;
  while((l != NULL) && (strcmp(l->name, name) != 0)) {
    l = l->next;
  }
  return l;
}

/* Add Dependency Target */
void add_dependency_target(target* tgt, char* dep) {
  addString(&(tgt->dep), dep);	
}

/* Add Rule Target */
void add_rule_target(target* tgt, char* rule) {
  addString(&(tgt->rule), rule);
}

/* For Each Dependency */
void for_each_dependency(target* tgt, void(*action)(char*)) {
  execDepRules(tgt->dep, action);
}


/* For Each Rule */
void for_each_rule(target* tgt, void(*action)(char*)) {
  for_each_string(tgt->rule, action);
}

int dependency_time_stamps(char* name, target* tgt) {
  return(check_timestamps(name, tgt->dep));
}
