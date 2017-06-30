#include "tokenize.h"
#include "object.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

int current = 0;
int unterminated_parens = 0;

nObj parse(list tokens) {
  if(current == length(tokens)) {
    current = 0;
    if(unterminated_parens) {
      puts("Parse error: unterminated parens");
      exit(1);
    }
    return NULL;
  }
  nObj result;
  Token* head = at(tokens,current);
  current++;
  switch(head->type) {
    case TK_QUOTE:
      result = new_empty_list();
      result->typedata.head = new_sym("'");
      result->typedata.head->next = parse(tokens);
      return result;
    case TK_LPAREN:
      result = new_empty_list();
      unterminated_parens++;
      result->typedata.head = parse(tokens);
      break;
    case TK_RPAREN:
      unterminated_parens--;
      return NULL;
    case TK_STR:
      result = new_str((char*)head->value);
      break;
    case TK_SYM:
      result = new_sym((char*)head->value);
      break;
    case TK_NUM:
      result = new_num(*((float*)head->value));
      break;
  }
  result->next = parse(tokens);
  return result;
}
/*
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
}*/
