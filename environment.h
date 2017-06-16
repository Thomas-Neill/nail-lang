#pragma once

#include "list.h"
#include "object.h"
#include "stdbool.h"

typedef struct KVpair {
  char* key;
  nObj value;
} KVpair;
  

typedef struct Environment {
  enum {GLOBAL,INNER} type;
  union {
    list global;
    struct {list inner; struct Environment *outer;} inner;
  } typedata;
} Environment;

bool has(const char* key,Environment e);
nObj* get(const char* key,Environment *e);
void free_env(Environment *e,bool global);
Environment new_global();
Environment* new_inner(Environment *outer);
//fetches the object if the key already exists, otherwise adds a new key/value pair and returns nullptr




