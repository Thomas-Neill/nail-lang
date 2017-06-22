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

nObj nailIf(nObj inputs) {
  if(!inputs) {puts("Three inputs expected for if function");exit(1);}
  if(!inputs->next) {puts("Three inputs expected for if function");exit(1);}
  if(!inputs->next->next) {puts("Three inputs expected for if function");exit(1);}
  if(inputs->next->next->next) {puts("Three inputs expected for if function");exit(1);}

  cache_eval_settings();
  eval_settings.just_eval_head = true;
  nObj cond = eval(inputs);
  reset_eval_settings();
  if(cond->type != BOOL) {puts("Expected boolean as first arg of if");exit(1);}
  bool b = cond->typedata.booldata;
  free_nObj(cond);

  nObj result;
  if(b) {
    cache_eval_settings();
    eval_settings.just_eval_head = true;
    result = eval(inputs->next);
    reset_eval_settings();
  } else {
    result = eval(inputs->next->next);
  }
  return result;
}

void load_stdlib() {
  set("+",new_magic_func(add));
  set("-",new_magic_func(sub));
  set("*",new_magic_func(mul));
  set("/",new_magic_func(divide));
  set("print!",new_magic_func(print));
  set("input?",new_magic_func(input));
  set("do",new_magic_func(doNAIL));
  set("lambda",new_magic_macro(make_function));

  set("set!",new_magic_macro(setsym));
  set("show!",new_magic_macro(show));
  set("enter-namespace",new_magic_macro(enter_namespace));
  set("if",new_magic_macro(nailIf));

  set("zilch",new_zilch());
  set("true",new_bool(true));
  set("false",new_bool(false));
}
