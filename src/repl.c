#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include <editline/readline.h>

// Possible Lisp values
enum { LVAL_NUM, LVAL_ERR };

// Possible error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Lisp types

typedef struct {
  int type;
  long num;
  int err;
} lval;

lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
  case LVAL_NUM: printf("%li", v.num); break;
  case LVAL_ERR:
    if (v.err == LERR_DIV_ZERO) { printf("Error: Division By Zero!"); }
    if (v.err == LERR_BAD_OP)   { printf("Error: Invalid Operator!"); }
    if (v.err == LERR_BAD_NUM)  { printf("Error: Invalid Number!"); }
    break;
  }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

// Evaluation

lval eval_op(lval x, char* operator, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(operator, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(operator, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(operator, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(operator, "%") == 0) { return lval_num(x.num % y.num); }
  if (strcmp(operator, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* operator = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, operator, eval(t->children[i]));
    i++;
  }
  return x;
}

// MAIN

int main(int argc, char** argv) {

  // Parsers

  mpc_parser_t* Number     = mpc_new("number");
  mpc_parser_t* Operator   = mpc_new("operator");
  mpc_parser_t* Expression = mpc_new("expression");
  mpc_parser_t* Lispy      = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                             \
      number     : /-?[0-9]+/ ;                                   \
      operator   : '+' | '-' | '*' | '/' | '%' ;                  \
      expression : <number> | '(' <operator> <expression>+ ')' ;  \
      lispy      : /^/ <operator> <expression>+ /$/ ;             \
    ",
    Number, Operator, Expression, Lispy);

  puts("Lispy version 0.0.1");
  puts("Press CTRL+C to exit\n");

  // Main loop

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }
  mpc_cleanup(4, Number, Operator, Expression, Lispy);
  return 0;
}
