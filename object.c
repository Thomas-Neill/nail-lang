#include "object.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "environment.h"

#define EMPTY malloc(sizeof(struct nailObject))

void debug_type(nailType n) {
  switch(n) {
    case LIST:
      puts("LIST");break;
    case STR:
      puts("STR");break;
    case SYM:
      puts("SYM");break;
    case NUM:
      puts("NUM");break;
    case MAGIC_FUNC:
      puts("MAGIC_FUNC");break;
    case MAGIC_MACRO:
      puts("MAGIC_MACRO");break;
    case USER_FUNC:
      puts("USER_FUNC");break;
    case ZILCH:
      puts("ZILCH");break;
    case BOOL:
      puts("BOOL");break;
  }
}

nObj new_str(const char* c) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = STR;
  result->typedata.strdata = strdup(c);
  result->next = NULL;
  return result;
}

nObj new_sym(const char* c) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = SYM;
  result->typedata.symdata = strdup(c);
  result->next = NULL;
  return result;
}

nObj new_num(float f) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = NUM;
  result->typedata.numdata = f;
  result->next = NULL;
  return result;
}

nObj new_bool(bool b) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = BOOL;
  result->typedata.booldata = b;
  result->next = NULL;
  return result;
}

nObj new_empty_list() {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = LIST;
  result->typedata.head = NULL;
  result->next = NULL;
  return result;
}

nObj new_zilch() {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = ZILCH;
  result->next = NULL;
  return result;
}

nObj new_magic_func(nObj (*func)(nObj)) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = MAGIC_FUNC;
  result->typedata.magic_func = func;
  result->next = NULL;
  return result;
}

nObj new_magic_macro(nObj (*func)(nObj)) {
  nObj result = new_magic_func(func);
  result->type = MAGIC_MACRO;
  return result;
}

nObj new_user_func(char **args, int nargs, nObj code,Environment* closure) {
  nObj result = EMPTY;
  result->quoted = false;
  result->type = USER_FUNC;
  result->next = NULL;
  result->typedata.func.argnames = args;
  result->typedata.func.nargs = nargs;
  result->typedata.func.code = code;
  result->typedata.func.closure = closure;
  result->typedata.func.ownsenv = true;
  return result;
}

nObj clone(nObj n,int clone_settings) {
  if(!n) return NULL;
  nObj result = EMPTY;
  result->quoted = n->quoted;
  result->type = n->type;
  if(clone_settings & JUST_CLONE_HEAD) {
    result->next = NULL;
  } else {
    result->next = clone(n->next,clone_settings);
  }
  switch(n->type) {
    case STR:
      result->typedata.strdata = strdup(n->typedata.strdata);
      break;
    case SYM:
      result->typedata.symdata = strdup(n->typedata.symdata);
      break;
    case LIST:
      result->typedata.head = clone(n->typedata.head,REGULAR);
      break;
    case USER_FUNC:;
      result->typedata = n->typedata;
      if(clone_settings & CHANGE_OWNERSHIP)  n->typedata.func.ownsenv = false;
      else result->typedata.func.ownsenv = false;
      break;
    default:
      result->typedata = n->typedata;
      break;
  }
  return result;
}

void free_nObj(nObj n) {
  if(!n) return;
  switch(n->type) {
    case STR:
      free(n->typedata.strdata);
      break;
    case SYM:
      free(n->typedata.symdata);
      break;
    case LIST:
      free_nObj(n->typedata.head);
      break;
    case USER_FUNC:
      if(n->typedata.func.ownsenv) {
        free_nObj(n->typedata.func.code);
        free_env(n->typedata.func.closure);
        char** args = n->typedata.func.argnames;
        for(int i = 0;i < n->typedata.func.nargs;i++) {
          free(args[i]);
        }
        free(args);
      }
      break;
    default: break;
  }
  free_nObj(n->next);
  free(n);
}

void out_nObj(nObj n) {
  switch(n->type) {
    case STR:
      printf("\"%s\"",n->typedata.strdata);
      break;
    case SYM:
      printf("%s",n->typedata.symdata);
      break;
    case NUM:
      printf("%f",n->typedata.numdata);
      break;
    case LIST:
      putchar('(');
      out_nObj(n->typedata.head);
      putchar(')');
      break;
    case BOOL:
      if(n->typedata.booldata) {
        printf("true");
      } else {
        printf("false");
      }
      break;
    case MAGIC_FUNC:
      printf("[Magic function]");
      break;
    case MAGIC_MACRO:
      printf("[Magic macro]");
      break;
    case ZILCH:
      printf("zilch");
      break;
    case USER_FUNC:
      printf("[User-defined function]");
      break;
  }
  if(!n->next) return;
  putchar(' '); //Items are seperated by spaces
  out_nObj(n->next);
}

static void aux_toStr(nObj n,char* result) {
  char temp[100];
  switch(n->type) {
    case STR:
      sprintf(temp,"\"%s\"",n->typedata.strdata);
      break;
    case SYM:
      sprintf(temp,"%s",n->typedata.symdata);
      break;
    case NUM:
      sprintf(temp,"%f",n->typedata.numdata);
      break;
    case LIST:
      strcat(result,"(");
      aux_toStr(n->typedata.head,result);
      strcat(result,")");
      break;
    case BOOL:
      if(n->typedata.booldata) {
        strcat(result,"true");
      } else {
        strcat(result,"false");
      }
      break;
    case MAGIC_FUNC:
      strcat(result,"[Magic function]");
      break;
    case MAGIC_MACRO:
      strcat(result,"[Magic macro]");
      break;
    case ZILCH:
      strcat(result,"zilch");
      break;
    case USER_FUNC:
      strcat(result,"[User function]");
      break;
  }
  strcat(result,temp);
  if(!n->next) return;
  strcat(result," ");
  aux_toStr(n->next,result);
}

char* toStr(nObj n,int bufsize) {
  char* result = malloc(bufsize);
  result[0] = '\0';
  aux_toStr(n,result);
  return result;
}
