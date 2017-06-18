#include "tokenize.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define peek() text[location+width]
//Helpers
static bool accept(const char* c);
static bool Naccept(const char* c);
static void push(TK_TYPE t);
static char* substr(const char *target,int start,int width); //completely unsafe: call with correct parameters or get a segfault
static void munch_token();

//global state
int location = 0;
int width = 0;
const char* text = NULL;
list tokens = NULL;

void reset_state() {
  location = 0;
  width = 0;
  tokens = NULL;
}

list tokenize(const char* text_) {
  reset_state();
  text = text_;
  while(location < strlen(text)) munch_token();
  return tokens;
}

static void munch_token() {
  if(accept("(")) {
    push(TK_LPAREN);
    return;
  } else if(accept(")")) {
    push(TK_RPAREN);
    return;
  } else if(accept("'")) {
    push(TK_QUOTE);
    return;
  } else if(accept("0123456789")) {
    while(accept("0123456789."));
    push(TK_NUM);
  } else if(accept("\n\t ")) {
    while(accept("\n\t ")) ;
    location += width;
    width = 0;
  } else if(Naccept("\"0123456789")) {
    while(Naccept("\"0123456789 \t\n()#"));
    push(TK_SYM);
  } else if(accept("\"")) {
    //TODO: add string escapes
    while(Naccept("\"\0"));
    if(peek() == '\0') {
      printf("Error: unterminated string literal!\n");
      free_tokens(tokens);
      exit(1);
    }
    accept("\"");
    push(TK_STR);
  } else {
    printf("Bad char: '%c'.\n",peek());
    free_tokens(tokens);
    exit(1);
}
}

static bool accept(const char* c) {
  if(strchr(c,peek())) {
    width++;
    return true;
  }
  return false;
}

static bool Naccept(const char* c) {
  if(!strchr(c,peek())) {
    width++;
    return true;
  }
  return false;
}

static void push(TK_TYPE t) {
  Token* new = malloc(sizeof(Token));
  new->type = t;
  new->text = substr(text,location,width);
  location += width;
  width=0;
  switch(t) {
    case TK_NUM: ;
      float* newF = malloc(sizeof(float));
      sscanf(new->text,"%f",newF);
      new->value = newF;
      break;
    case TK_SYM:
      new->value = malloc(strlen(new->text) + 1);
      strcpy(new->value,new->text);
      break;
    case TK_STR:
      new->value = substr(new->text,1,strlen(new->text)-2);
      break;
    default:
      new->value = NULL;
      break;
  }
  append(new,&tokens);
}

static char* substr(const char* text,int location,int width) {
  char* result = malloc(sizeof(char) * width + 1);
  for(int index = 0;index < width;index++) {
    result[index] = text[location + index];
  }
  result[width] = '\0';
  return result;
}

char* textify_token(Token *tk) {
  char *result = malloc(50);
  switch(tk->type) {
    case TK_LPAREN:
      strcpy(result,"<token \"(\">");
      break;
    case TK_RPAREN:
      strcpy(result,"<token \")\">");
      break;
    case TK_QUOTE:
      strcpy(result,"<token \"'\">");
      break;
    case TK_NUM:
      sprintf(result,"<number %f>",*((float*)tk->value));
      break;
    case TK_SYM:
      sprintf(result,"<symbol \"%s\">",(char*)tk->value);
      break;
    case TK_STR:
      sprintf(result,"<string \"%s\">",(char*)tk->value);
      break;
  }
  return result;
}

void print_tokens(list tks) {
  char* current;
  while(tks != NULL) {
    current = textify_token(tks->head);
    printf("%s\n",current);
    free(current);
    tks = tks->tail;
  }
}

void free_token(void *x) {
  Token* t = (Token*)x;
  free(t->text);
  free(t->value);
  free(x);
}

void free_tokens(list l) {
  delWith(l,free_token);
}
/*
int main() {
  char buffer[100];
  while(true) {
    printf("test> ");
    fgets(buffer,100,stdin);
    list result = tokenize(buffer);
    print_tokens(result);
    free_tokens(result);
  }
}*/
