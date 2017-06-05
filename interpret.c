#include "interpret.h"
#include "stdio.h"
#include "tokenize.h"
#include "parse.h"
#include "string.h"

nObj eval(nObj n) {
  if(!n) return NULL;
  nObj result;
  if(n->quoted) {
    result = clone(n);
    free_nObj(result->next);
    result->next = eval(n->next);
    return result;
  } else {
    switch(n->type) {
      case LIST: 
        result = call(n);
        break;
      case SYM:
        result = clone(*(get(n->typedata.symdata,&global)));
        if(result == NULL) {
          printf("Variable '%s' doesn't exist!\n",n->typedata.symdata);
          exit(1);
        }
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

nObj call(nObj l) {
  nObj func = eval(l->typedata.head);
  nObj result;
  switch(func->type) {
    case MAGIC_FUNC:;
      nObj input = func->next;
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

void set(char* c,nObj n) {
  *(get(c,&global)) = n;
}

//TODO: actually handle errors rather than just exit(1)'ing
nObj add(nObj inputs) {
  if(!inputs) {
    printf("Inputs required for '+'\n");
    exit(1);
  }
  if(inputs->type != NUM) {
      printf("Invalid type for '+' operation\n");
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
  set(inputs->typedata.symdata,clone(inputs->next));
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
int main() {
  set("+",new_magic_func(add));
  set("-",new_magic_func(sub));
  set("*",new_magic_func(mul));
  set("/",new_magic_func(divide));
  set("zilch",new_zilch());
  set("set!",new_magic_func(setsym));
  set("print!",new_magic_func(print));
  set("input?",new_magic_func(input));
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
    result = eval(ast);
    out_nObj(result); putchar('\n');
    free_tokens(tks); 
    free_nObj(result);
    free_nObj(ast);
    text[0] = '\0';
  }
  free_env(global);
}
