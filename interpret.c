#include "interpret.h"
#include "stdio.h"
#include "string.h"


nObj eval(nObj n) {
  if(!n) return NULL;
  nObj result;
  if(n->quoted) {
    result = clone(n);
    free_nObj(result->next);
  } else {
    switch(n->type) {
      case LIST:
        result = call(n);
        break;
      case SYM:
        result = clone(*(get(n->typedata.symdata,scope)));
        if(result == NULL) {
          printf("Variable '%s' doesn't exist!\n",n->typedata.symdata);
          exit(1);
        }
        free_nObj(result->next);
        break;
      default:
        result = clone(n);
        free_nObj(result->next);
       break;
     }
  }
  result->next = eval(n->next);
  return result;
}

nObj call(nObj l) {
  //just evaluate the head!!!
  nObj copy = clone(l);
  nObj temp = copy->typedata.head->next;
  copy->typedata.head->next = NULL;
  nObj func = eval(copy->typedata.head);
  copy->typedata.head->next = temp;
  free_nObj(copy);

  nObj inputs = l->typedata.head->next;
  nObj result;
  switch(func->type) {
    case MAGIC_FUNC:
      temp = eval(inputs);
      result = func->typedata.magic_func(temp);
      free_nObj(temp);
      break;
    case MAGIC_MACRO:
      result = func->typedata.magic_func(inputs);
      break;
    case USER_FUNC:
      temp = eval(inputs);
      result = call_user_func(func,temp);
      free_nObj(temp);
      break;
    default:
      printf("Invalid object type called as function\n");
      exit(1);
  }
  free_nObj(func);
  return result;
}


nObj call_user_func(nObj func,nObj inputs) {
  Environment* old_scope = scope;
  scope = func->typedata.func.closure;
  nObj ptr = clone(inputs);
  for(int i = 0;i < func->typedata.func.nargs;i++) {
    if(!ptr) {puts("insufficent inputs!");exit(1);}
    nObj temp = ptr->next;
    ptr->next = NULL;
    set(func->typedata.func.argnames[i],ptr);
    ptr = temp;
  }
  nObj result = eval(func->typedata.func.code);
  scope = old_scope;
  return result;
}

void set(char* c,nObj n) {
  nObj* val = get(c,scope);
  if((*val) != NULL) free_nObj(*val);
  (*val) = n;
}

void init_interpreter() {
  reset_clone_settings();
  global = new_global();
  scope = &global;
}

void free_interpreter() {
  free_env(&global);
}
