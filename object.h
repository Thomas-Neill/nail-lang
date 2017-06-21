#pragma once
#include "stdbool.h"
#include "stdlib.h"

typedef enum {LIST,STR,SYM,NUM,MAGIC_FUNC,MAGIC_MACRO,USER_FUNC,ZILCH} nailType;
void debug_type (nailType n);
struct Environment;

typedef struct nailObject {
  struct nailObject* next;
  nailType type;
  union {
    char* strdata; //STR
    char* symdata; //SYM
    float numdata; //NUM
    struct nailObject* head; //LIST
    struct nailObject* (*magic_func)(struct nailObject*); //MAGIC_FUNC,MAGIC_MACRO
    struct {char** argnames; int nargs; struct nailObject* code; struct Environment *closure; bool ownsenv;} func; //USER_FUNC
  } typedata;
  bool quoted;
}* nObj;

nObj new_str(const char* c);
nObj new_sym(const char* c);
nObj new_num(float f);
nObj new_empty_list();
nObj new_magic_func(nObj (*func)(nObj));
nObj new_magic_macro(nObj (*func)(nObj));
nObj new_user_func(char **args, int nargs, nObj code,struct Environment *closure);
nObj new_zilch();

struct {bool change_ownership;} clone_settings;
void reset_clone_settings();
nObj clone(nObj n);

void free_nObj(nObj n);

void out_nObj(nObj n);
char* toStr(nObj n,int bufSize);
