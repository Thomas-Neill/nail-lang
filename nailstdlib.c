#include "interpret.h"
#include "object.h"
#include "stdio.h"
#include "string.h"

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
  set(inputs->typedata.symdata,eval(inputs->next,CLONE_CHANGE_OWNERSHIP));
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
  return clone(sideffects,REGULAR);
}


nObj show(nObj input) {
  out_nObj(input);putchar('\n');
  return new_zilch();
}

nObj enter_namespace(nObj operation) {
  Environment* last = scope;
  scope = new_inner(scope);
  free_nObj(eval(operation,REGULAR));
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
  return new_user_func(extractedargs,nargs,clone(args->next,REGULAR),new_closure(scope));
}

nObj nailIf(nObj inputs) {
  if(!inputs) {puts("Three inputs expected for if function");exit(1);}
  if(!inputs->next) {puts("Three inputs expected for if function");exit(1);}
  if(!inputs->next->next) {puts("Three inputs expected for if function");exit(1);}
  if(inputs->next->next->next) {puts("Three inputs expected for if function");exit(1);}

  nObj cond = eval(inputs,JUST_EVAL_HEAD);
  if(cond->type != BOOL) {puts("Expected boolean as first arg of if");exit(1);}
  bool b = cond->typedata.booldata;
  free_nObj(cond);

  nObj result;
  if(b) {
    result = eval(inputs->next,JUST_EVAL_HEAD);
  } else {
    result = eval(inputs->next->next,JUST_EVAL_HEAD);
  }
  return result;
}

nObj equals(nObj inputs) {
  if(!inputs) {puts("Two inputs expected for = function.");exit(1);}
  if(!inputs->next) {puts("Two inputs expected for = function.");exit(1);}
  nObj x = inputs;
  nObj y = inputs->next;
  if(y->type != x->type) {
    return new_bool(false);
  }
  switch(x->type) {
    case NUM:
      return new_bool(x->typedata.numdata == y->typedata.numdata);
    case MAGIC_FUNC:
    case MAGIC_MACRO:
      return new_bool(x->typedata.magic_func == y->typedata.magic_func);
    case STR:
    case SYM:
      return new_bool(!strcmp(x->typedata.strdata,y->typedata.strdata));
    case ZILCH:
      return new_bool(true);
    case LIST:
      x = x->typedata.head;
      y = y->typedata.head;
      nObj pair,result;
      while(x && y) {
        pair = clone(x,JUST_CLONE_HEAD);
        pair->next = clone(y,JUST_CLONE_HEAD);
        result = equals(pair);
        bool b = result->typedata.booldata;
        free_nObj(result);
        free_nObj(pair);
        if(!b) {
          return new_bool(false);
        }
        x = x->next;
        y = y->next;
      }
      //inequal length
      if(((bool)x) != ((bool)y)) return new_bool(false);
      return new_bool(true);
    default:
      return new_bool(false);
  }
}

void load_stdlib() {
  set("+",new_magic_func(add));
  set("-",new_magic_func(sub));
  set("*",new_magic_func(mul));
  set("/",new_magic_func(divide));
  set("print!",new_magic_func(print));
  set("input?",new_magic_func(input));
  set("do",new_magic_func(doNAIL));
  set("=",new_magic_func(equals));

  set("set!",new_magic_macro(setsym));
  set("show!",new_magic_macro(show));
  set("enter-namespace",new_magic_macro(enter_namespace));
  set("if",new_magic_macro(nailIf));
  set("lambda",new_magic_macro(make_function));

  set("zilch",new_zilch());
  set("true",new_bool(true));
  set("false",new_bool(false));
}
