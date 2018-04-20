#include "umakeString.h"
#include "target.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

struct string_st {
  string* next;
  char* name;
};

typedef string* string_list;

/* Add String */
void addString(string_list* s, char* name) {
  while(*s != NULL) {
    s = &((*s)->next);
  }
  *s = new_string(name);
}

/* New String */
string* new_string(char* name) {
  string* s = malloc(sizeof(string));
  if (s != NULL) {
    s->next = NULL;
    s->name = strdup(name);
  }
  return s;
}


/* For Each String */
void for_each_string(string_list list, void(*action)(char*)) {
  while (list != NULL) {
    action(list->name);
    list = list->next;
  }
}

/* Get String Name */
char* getStringName(string* str) {
  return str->name;
}

/* Exec Dep Rules */
void execDepRules(string_list list, void(*action)(char*)) {
  while (list != NULL) {
    target* currTarget = find_target(list->name);
    if (currTarget != NULL) {
      for_each_dependency(currTarget, action);
      FILE* file = fopen(list->name, "r");
      int timestamp = dependency_time_stamps(list->name, currTarget);
      if ((file == NULL) || (timestamp == 1)) {
        for_each_rule(currTarget, action);
      }
    }
    list = list->next;
  }
}

int check_timestamps(char* name, string_list list) {
  struct stat targetTime;
  struct stat times;
  int statNum = stat(name, &targetTime);
  if (statNum != 0) {
    return 0;
  }
  long targetMod = targetTime.st_mtime;
  while (list != NULL) {
    statNum = stat(list->name, &times);
    if (statNum != 0) {
      list = list->next;
      continue;
    }
    long depMod = times.st_mtime;
    if (depMod > targetMod) {
      return 1;
    }
    list = list->next;
  }
  return 0;
}




