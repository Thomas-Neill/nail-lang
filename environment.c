#include "environment.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

static bool has_aux(const char* key,list e) {
  while(e != NULL) {
    if(strcmp(key,((KVpair*)e->head)->key) == 0) return true;
    e = e->tail;
  }
  return false;
}

bool has(const char* key,Environment e) {
  bool result;
  switch(e.type) {
    case GLOBAL:
      result = has_aux(key,e.typedata.global);
      break;
    case INNER:
      result = has_aux(key,e.typedata.inner.inner->kvpairs) || has(key,*e.typedata.inner.outer);
      break;
  }
  return result;
}

static nObj* get_aux(const char* key,list* e) {
  if((*e) == NULL) {
    KVpair* new = malloc(sizeof(KVpair));
    new->key = malloc(strlen(key)+1);
    strcpy(new->key,key);
    new->value = NULL;
    prepend(new,e);
    return &new->value;
  } else if(strcmp(key,((KVpair*)(*e)->head)->key) != 0) { //Different keys
    return get_aux(key,&(*e)->tail);
  } else {
    return &((KVpair*)(*e)->head)->value;
  }
}

nObj* get(const char* key,Environment *e) {
  nObj* result;
  switch(e->type) {
    case GLOBAL:
      result = get_aux(key,&e->typedata.global);
      break;
    case INNER:
      if(has_aux(key,e->typedata.inner.inner->kvpairs)) { //Variable exists inside scope
        result = get_aux(key,&e->typedata.inner.inner->kvpairs);
      } else if(has(key,*e->typedata.inner.outer)) { //Variable exists outside scope
        result = get(key,e->typedata.inner.outer);
      } else { //Variable doesn't exist
        result = get_aux(key,&e->typedata.inner.inner->kvpairs);
      }
      break;
  }
  return result;
}

static void freeKV(void* kv) {
  KVpair* k = (KVpair*)kv;
  free_nObj(k->value);
  free(k->key);
  free(kv);
}

static void free_env_aux(list e) {
  delWith(e,freeKV);
}

void free_env(Environment *e) {
  puts("attempted murder");
  switch(e->type) {
    case GLOBAL:
      free_env_aux(e->typedata.global);
      break;
    case INNER:
      if(e->typedata.inner.inner->refcount == 1) { //this is the last reference to it
        puts("killing an environment");
        free_env_aux(e->typedata.inner.inner->kvpairs);
        free(e->typedata.inner.inner);
        free(e);
      } else {
        e->typedata.inner.inner->refcount--;
      }
      break;
  }
}

Environment new_global() {
  Environment result;
  result.type = GLOBAL;
  result.typedata.global = NULL;
  return result;
}

Environment* new_inner(Environment *outer) {
  Environment *result = malloc(sizeof(Environment));
  result->type = INNER;
  result->typedata.inner.inner = malloc(sizeof(Namespace));
  result->typedata.inner.inner->refcount = 1;
  result->typedata.inner.inner->kvpairs = NULL;
  result->typedata.inner.outer = outer;
  return result;
}

Environment* new_closure(Environment *outer) {
  Environment* result = new_inner(outer);
  while(outer->type == INNER) { //Increase reference counts
    outer->typedata.inner.inner->refcount++;
    outer = outer->typedata.inner.outer;
  }
  return result;
}
