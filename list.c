#include "list.h"
#include <stdlib.h>

void prepend(void* x,list *l) {
  list result = malloc(sizeof(struct list));
  result->head = x;
  result->tail = (*l);
  (*l) = result;
}

void append(void* x,list* l) {
  if((*l) == NULL) {
    (*l) = malloc(sizeof(struct list));
    (*l)->tail = NULL;
    (*l)->head = x;
  } else {
    append(x,&((*l)->tail));
  }
}

void* at(list l,int n) {
  if(l == NULL) return NULL;
  if(n == 0) return l->head;
  return at(l->tail,n-1);
}

void delWith(list l,void(*fn)(void*)) {
  if(l == NULL) return; //our work is done here
  //delete the head
  fn(l->head);
  //delete the remainder's contents
  delWith(l->tail,fn);
  //delete the leftovers
  free(l);
}

void del(list l) {
  delWith(l,free);  
}


int length(list l) {
  if(l == NULL) return 0;
  return 1 + length(l->tail);
}
