#pragma once
#include "object.h"
#include "environment.h"

Environment global;
Environment* scope;

nObj eval(nObj n);
nObj call(nObj list);
nObj call_user_func(nObj func,nObj inputs);
void set(char *c,nObj n);
void init_interpreter();
void free_interpreter();
