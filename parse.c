#include "tokenize.h"
#include "object.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

int current = 0;

nObj parse(list tokens) {
  if(current == length(tokens)) {
    current = 0; 
    return NULL;
  }
  nObj result = malloc(sizeof(struct nailObject));
  Token* head = at(tokens,current);
  current++;
  switch(head->type) {
    case TK_HASH:
      free(result);
      return parse(tokens);
    case TK_LPAREN:
      result->type = LIST;
      result->typedata.head = parse(tokens);
      break;
    case TK_RPAREN:
      free(result);
      return NULL;
    case TK_STR:
      result->type = STR;
      result->typedata.strdata = strdup((char*)head->value);
      break;
    case TK_SYM:
      result->type = SYM;
      result->typedata.symdata = strdup((char*)head->value);
      break;
    case TK_NUM:
      result->type = NUM;
      result->typedata.numdata = *((float*)head->value);
      break;
  }
  result->next = parse(tokens);
  return result;
}

int main() {
  char buffer[100] = "";
  while(1) {
    fgets(buffer,100,stdin);
    list tokens = tokenize(buffer);
    nObj result = parse(tokens);
    out_nObj(result);
    putchar('\n');
    free_tokens(tokens);
    free_nObj(result);
  }
}
