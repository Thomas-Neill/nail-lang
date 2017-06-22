#include "interpret.h"
#include "stdio.h"
#include "tokenize.h"
#include "parse.h"
#include "string.h"
#include "editline/readline.h"
#include "editline/history.h"

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

nObj call_user_func(nObj func,nObj args);

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

void set(char* c,nObj n);

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

//BEGIN testing

void set(char* c,nObj n) {
  nObj* val = get(c,scope);
  if((*val) != NULL) free_nObj(*val);
  (*val) = n;
}

//TODO: actually handle errors rather than just exit(1)'ing
nObj add(nObj inputs) {
  if(!inputs) {
    printf("Inputs required for '+'\n");
    exit(1);
  }
  if(inputs->type != NUM) {
      printf("Invalid type for '+' operation:\n");
      exit(1);
  }
  if(!inputs->next) return new_num(abs(inputs->typedata.numdata));
  float sum = 0;
  while(inputs) {
    if(inputs->type != NUM) {
      printf("Invalid type for '+' operation\n");
      exit(1);
    }
    sum += inputs->typedata.numdata;
    inputs = inputs->next;
  }
  return new_num(sum);
}

nObj sub(nObj inputs) {
  if(!inputs) {
    printf("Inputs required for '-'\n");
    exit(1);
  }
  if(!inputs->next) return new_num(-abs(inputs->typedata.numdata));
  if(inputs->type != NUM || inputs->next->type != NUM) {
    printf("Invalid type for '-' operation\n");
    exit(1);
  }
  return new_num(inputs->typedata.numdata-inputs->next->typedata.numdata);
}

nObj mul(nObj inputs) {
  if(!inputs) {
    printf("Inputs required for '*'\n");
    exit(1);
  }
  float sum = 1;
  while(inputs) {
    if(inputs->type != NUM) {
      out_nObj(inputs);putchar('\n');
      printf("Invalid type for '*' operation\n");
      exit(1);
    }
    sum *= inputs->typedata.numdata;
    inputs = inputs->next;
  }
  return new_num(sum);
}

nObj divide(nObj inputs) {
  if(!inputs) {
    printf("Inputs required for '/'\n");
    exit(1);
  }
  if(!inputs->next) return new_num(-abs(inputs->typedata.numdata));
  if(inputs->type != NUM || inputs->next->type != NUM) {
    printf("Invalid type for '/' operation\n");
    exit(1);
  }
  return new_num(inputs->typedata.numdata/inputs->next->typedata.numdata);
}

nObj setsym(nObj inputs) {
  clone_settings.change_ownership = true;
  set(inputs->typedata.symdata,eval(inputs->next));
  reset_clone_settings();
  return new_zilch();
}

nObj print(nObj inputs) {
  if(inputs && !inputs->next) {
    if(inputs->type != STR) {puts("Invalid type to print!"); exit(1);}
    puts(inputs->typedata.strdata);
    return new_zilch();
  } else {
    puts("Too many or too few args");
    exit(1);
  }
}

nObj input(nObj useless) {
  char buffer[500];
  fgets(buffer,500,stdin);
  for(int i = 0; 500 > i; i++) if(buffer[i] == '\n') {buffer[i] = '\0'; break;}
  return new_str(buffer);
}

nObj doNAIL(nObj sideffects) {
  if(!sideffects) return new_zilch();
  while(sideffects->next) {
    sideffects = sideffects->next;
  }
  return clone(sideffects);
}


nObj show(nObj input) {
  out_nObj(input);putchar('\n');
  return new_zilch();
}

nObj enter_namespace(nObj operation) {
  Environment* last = scope;
  scope = new_inner(scope);
  free_nObj(eval(operation));
  free_env(scope);
  scope = last;
  return new_zilch();
}

nObj make_function(nObj args) {
  int nargs = 0;
  char** extractedargs;
  //extract number of arguments to function
  if(args->type != LIST) {puts("Expected argument list as first argument"); exit(1);}
  nObj temp = args->typedata.head;
  while(temp) {
    if(temp->type != SYM) {puts("Expected symbol in symbol list."); exit(1);}
    temp = temp->next;
    nargs++;
  }
  //extract names of arguments
  extractedargs = malloc(sizeof(char*)*nargs);
  temp = args->typedata.head;
  int ind = 0;
  while(temp) {
    extractedargs[ind] = strdup(temp->typedata.symdata);
    ind++;
    temp = temp->next;
  }
  return new_user_func(extractedargs,nargs,clone(args->next),new_closure(scope));
}

int main() {
  reset_clone_settings();
  global = new_global();
  scope = &global;

  set("+",new_magic_func(add));
  set("-",new_magic_func(sub));
  set("*",new_magic_func(mul));
  set("/",new_magic_func(divide));
  set("zilch",new_zilch());
  set("print!",new_magic_func(print));
  set("input?",new_magic_func(input));
  set("do",new_magic_func(doNAIL));
  set("lambda",new_magic_macro(make_function));

  set("set!",new_magic_macro(setsym));
  set("show!",new_magic_macro(show));
  set("enter-namespace",new_magic_macro(enter_namespace));

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
    result = eval(ast);

    out_nObj(result); putchar('\n');

    free_tokens(tks);
    free_nObj(result);
    free_nObj(ast);
    free(input);
  }
  free_env(&global);
}
