#pragma once
#include "object.h"
#include "environment.h"

Environment global;
Environment* scope;

nObj eval(nObj n,bool just_head);
nObj call(nObj list);
