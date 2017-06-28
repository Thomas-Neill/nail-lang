#pragma once
#include "object.h"
#include "environment.h"

Environment global;
Environment* scope;

enum {JUST_EVAL_HEAD = 1, CLONE_CHANGE_OWNERSHIP = 2};
nObj eval(nObj n,int eval_settings);
nObj call(nObj list);
nObj call_user_func(nObj func,nObj inputs);
void set(char *c,nObj n);
void set_inner(char* c,nObj n);
void init_interpreter();
void free_interpreter();
