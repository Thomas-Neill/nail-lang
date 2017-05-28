#pragma once

typedef enum {LIST,STR,SYM,NUM} nailType;

typedef struct nailObject {
  struct nailObject* next;
  nailType type;
  union {
    char* strdata; //STR
    char* symdata; //SYM
    float numdata; //NUM
    struct nailObject* head; //LIST
  } typedata;
}* nObj;

nObj new_str(const char* c);
nObj new_sym(const char* c);
nObj new_num(float f);
nObj new_list(nObj* array,int length);

void free_nObj(nObj n);

void out_nObj(nObj n);
char* toStr(nObj n,int bufSize);
