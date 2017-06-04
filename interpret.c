#include "interpret.h"
#include "stdio.h"
#include "tokenize.h"
#include "parse.h"
#include "string.h"

nObj eval(nObj n) {
  if(!n) return NULL;
  if(n->quoted) return clone(n);
  nObj result;
  switch(n->type) {
    case LIST: ;
      result = call(n->typedata.head,n->typedata.head->next);
      result->next = eval(n->next);
      break;
    case SYM: ;
      result = *(get(n->typedata.symdata,&global));
      if(result == NULL) {
        printf("Variable '%s' doesn't exist!\n",n->typedata.symdata);
        exit(1);
      }
      result->next = eval(n->next);
      break;
    default:
      result = clone(n);
      free_nObj(result->next);
      result->next = eval(n->next);
      break;
  }
  return result;
}

nObj call(nObj func,nObj input) {
  func = eval(func);
  nObj result;
  switch(func->type) {
    case MAGIC_FUNC:
      input = eval(input);
      result = func->typedata.magic_func(input);
      free_nObj(input);
      break;
    default:
      printf("Invalid object type called as function\n");
      exit(1);
  }
  return result;
}

//BEGIN testing

nObj add(nObj inputs) {
  float sum = 0;
  while(inputs) {
    if(inputs->type != NUM) {
      printf("Invalid type for 'add' operation\n");
      exit(1);
    }
    sum += inputs->typedata.numdata;
    inputs = inputs->next;
  }
  return new_num(sum);
}

void set(char* c,nObj n) {
  *(get(c,&global)) = n;
}
int main() {
  set("+",new_magic_func(add));
  set("foobar",new_num(5));
  list tks;
  nObj ast;
  nObj result;
  char text[500] = "";
  while(true) {
    printf("NAIL> ");
    fgets(text,500,stdin);
    if(strncmp("quit",text,4) == 0) break;
    tks = tokenize(text);
    ast = parse(tks);
    free_tokens(tks);
    result = eval(ast);
    out_nObj(result); putchar('\n');
    free_nObj(result);
    free_nObj(ast);
  }
  free_env(global);
}
