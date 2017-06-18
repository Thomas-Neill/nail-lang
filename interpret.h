#pragma once
#include "object.h"
#include "environment.h"

Environment global;
Environment* scope;

nObj eval(nObj n);
nObj call(nObj list);
