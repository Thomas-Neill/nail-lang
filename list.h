#pragma once

struct list {
  void* head;
  struct list* tail;
};

typedef struct list RAW_LIST;
typedef struct list* list;

//list operations
void prepend(void* x,list* l);
//Note: appending is an O(N) vs the O(1) of prepend.
void  append(void* x,list* l);

void* at(list l,int n);

void del(list l);
void delWith(list l,void(*fn)(void*));
int length(list l);
