#pragma once
#include "stdbool.h"
#include "stdlib.h"

typedef enum {LIST,STR,SYM,NUM,MAGIC_FUNC,MAGIC_MACRO,USER_FUNC,USER_MACRO,ZILCH,BOOL} nailType;
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
    struct {
      char** argnames;
      int nargs;
      struct nailObject* code;
      struct Environment *closure;
      bool ownsenv;} func;
    //USER_FUNC,USER_MACRO
    bool booldata; //BOOL
  } typedata;
  bool quoted;
}* nObj;

nObj new_str(const char* c);
nObj new_sym(const char* c);
nObj new_num(float f);
nObj new_empty_list();
nObj new_magic_func(nObj (*func)(nObj));
nObj new_magic_macro(nObj (*func)(nObj));
nObj new_bool(bool b);
nObj new_user_func(char **args, int nargs, nObj code,struct Environment *closure);
nObj new_user_macro(char **args,int nargs,nObj code,struct Environment *closure);
nObj new_zilch();

enum {REGULAR = 0, JUST_CLONE_HEAD = 1, CHANGE_OWNERSHIP = 2};
nObj clone(nObj n,int clone_settings);

void free_nObj(nObj n);

void out_nObj(nObj n);
char* toStr(nObj n,int bufSize);
