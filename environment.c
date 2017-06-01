#include "environment.h"
#include "string.h"
#include "stdlib.h"

nObj* get(const char* key,Environment* e) {
  if((*e) == NULL) {
    KVpair* new = malloc(sizeof(KVpair));
    new->key = malloc(strlen(key)+1);
    strcpy(new->key,key);
    new->value = NULL;
    prepend(new,e);
    return &new->value;
  } else if(strcmp(key,((KVpair*)(*e)->head)->key) != 0) { //Different keys
    return get(key,&(*e)->tail);
  } else //matching keys
    return &((KVpair*)(*e)->head)->value;
  
}

bool has(const char* key,Environment e) {
  while(e != NULL) {
    if(strcmp(key,((KVpair*)e->head)->key) == 0) return true;
    e = e->tail;
  }
  return false;
}
static void freeKV(void* kv) {
  KVpair* k = (KVpair*)kv;
  free_nObj(k->value);
  free(k->key);
  free(kv);
}
void free_env(Environment e) {
  delWith(e,freeKV);
}
int main() {
  Environment test = NULL;
  (*get("foo",&test)) = new_num(5);
  out_nObj(*get("foo",&test));
  free_env(test);
}
