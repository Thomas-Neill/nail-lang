#pragma once
#include "object.h"
#include "environment.h"

Environment global;
Environment* scope;

typedef struct {bool just_eval_head;} __eval_settings;
__eval_settings eval_settings;
void reset_eval_settings();
void cache_eval_settings();
nObj eval(nObj n);
nObj call(nObj list);
nObj call_user_func(nObj func,nObj inputs);
void set(char *c,nObj n);
void init_interpreter();
void free_interpreter();
