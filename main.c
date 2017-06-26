#include "interpret.h"
#include "tokenize.h"
#include "parse.h"
#include "nailstdlib.h"
#include "editline/readline.h"
#include "editline/history.h"
#include "string.h"

void run_REPL() {
  list tks;
  nObj ast;
  nObj result;
  char *input;
  while(true) {
    input = readline("NAIL> ");
    add_history(input);
    if(!strcmp(input,"quit")) {free(input);break;}

    tks = tokenize(input);
    ast = parse(tks);
    result = eval(ast,REGULAR);

    out_nObj(result); putchar('\n');

    free_tokens(tks);
    free_nObj(result);
    free_nObj(ast);
    free(input);
  }
}


void exec_file(char* name) {
  char buf[1000];
  FILE* f = fopen(name,"r");
  fgets(buf,1000,(FILE*)f);
  fclose(f);
  list tks = tokenize(buf);
  nObj ast = parse(tks);
  free_nObj(eval(ast,REGULAR));
  free_nObj(ast);
  free_tokens(tks);
}

int main(int argv,char** argc) {
  init_interpreter();
  load_stdlib();
  if(argv == 1) {
    run_REPL();
  } else if(argv == 2) { //filename to execute
    exec_file(argc[1]);
  }
  free_interpreter();
  return 0;
}
