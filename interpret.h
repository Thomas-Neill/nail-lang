#pragma once
#include "object.h"
#include "environment.h"

Environment global = NULL;

nObj eval(nObj n);
nObj call(nObj func,nObj input);
