#pragma once
#include "list.h"

typedef enum {TK_LPAREN,TK_RPAREN,TK_HASH, // # is equivalent to ' in other lisps
	TK_SYM,TK_STR,TK_NUM //Atoms
} TK_TYPE;

typedef struct Token {
	TK_TYPE type;
	char* text;
	void* value;
} Token;

list tokenize(const char* text);
char* textify_token(Token* tk);
void free_tokens(list l);
