#pragma once

#include "list.h"
#include "object.h"
#include "stdbool.h"


typedef struct KVpair {
  char* key;
  nObj value;
} KVpair;

typedef struct namespace {
  list kvpairs;
  int refcount;
} Namespace;

typedef struct Environment {
  enum {GLOBAL,INNER} type;
  union {
    list global;
    struct {Namespace *inner; struct Environment *outer;} inner;
  } typedata;
} Environment;

bool has(const char* key,Environment e);
nObj* get(const char* key,Environment *e);
nObj* get_inner(const char* key,Environment *e); //force access at inner scope
Environment* clone_env(Environment* e);

void free_env(Environment *e);

Environment new_global();
Environment* new_inner(Environment *outer);
Environment* new_closure(Environment *outer);
//fetches the object if the key already exists, otherwise adds a new key/value pair and returns its address;
