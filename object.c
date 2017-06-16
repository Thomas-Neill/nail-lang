#include "object.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define EMPTY malloc(sizeof(struct nailObject))

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

nObj clone(nObj n) {
  if(!n) return NULL;
  nObj result = EMPTY;
  result->quoted = n->quoted;
  result->type = n->type;
  result->next = clone(n->next);
  switch(n->type) {
    case STR:
      result->typedata.strdata = strdup(n->typedata.strdata);
      break;
    case SYM:
      result->typedata.symdata = strdup(n->typedata.symdata);
      break;
    case LIST:
      result->typedata.head = clone(n->typedata.head);
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
    default:
      break;
  }
  free_nObj(n->next);
  free(n);
}

void out_nObj(nObj n) {
  if(n->quoted) putchar('#');
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
    case MAGIC_FUNC:
      printf("[Magic function]");
      break;
    case MAGIC_MACRO:
      printf("[Magic macro]");
      break;
    case ZILCH:
      printf("zilch");
      break;
  }
  if(!n->next) return;
  putchar(' '); //Items are seperated by spaces
  out_nObj(n->next);
}

static void aux_toStr(nObj n,char* result) {
  if(n->quoted) strcat(result,"#");
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
    case MAGIC_FUNC:
      strcat(result,"[Magic function]");
      break;
    case MAGIC_MACRO:
      strcat(result,"[Magic macro]");
      break;
    case ZILCH:
      strcat(result,"zilch");
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
