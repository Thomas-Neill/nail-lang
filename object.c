#include "object.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define EMPTY malloc(sizeof(struct nailObject))

nObj new_str(const char* c) {
  nObj result = EMPTY;
  result->type = STR;
  result->typedata.strdata = strdup(c);
  result->next = NULL;
  return result;
}

nObj new_sym(const char* c) {
  nObj result = EMPTY;
  result->type = SYM;
  result->typedata.symdata = strdup(c);
  result->next = NULL;
  return result;
}

nObj new_num(float f) {
  nObj result = EMPTY;
  result->type = NUM;
  result->typedata.numdata = f;
  result->next = NULL;
  return result;
}

nObj new_list(nObj* array,int length) {
  for(int i = 0;i < length-1;i++) {
    array[i]->next = array[i+1];
  }
  array[length-1]->next = NULL;
  nObj result = EMPTY;
  result->type = LIST;
  result->typedata.head = array[0];
  result->next = NULL;
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
