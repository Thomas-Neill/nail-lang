#pragma once

#include "list.h"
#include "object.h"
#include "stdbool.h"

typedef struct KVpair {
  char* key;
  nObj value;
} KVpair;
  

typedef list Environment;

bool has(const char* key,Environment e);
nObj* get(const char* key,Environment* e);
void free_env(Environment e);
//fetches the object if the key already exists, otherwise adds a new key/value pair and returns nullptr




