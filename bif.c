#include "main.h"

/* BIF - built-in forms */

#define BIF(cname) word bif_##cname(word args, word env)

#define DEF_BIF(cname, name)      \
  BIF_FNS[BIF_LEN] = bif_##cname; \
  if (add_to_img) SYM_VAL(intern(name, MAIN)) = BIF_REF(BIF_LEN++)

#define EVAL_ALL()                                  \
  word eargs = NIL;                                 \
  while (args != NIL) {                             \
    eargs = cons(eval(CONS_CAR(args), env), eargs); \
    args = CONS_CDR(args);                          \
  }                                                 \
  args = eargs

word BIF_LEN = 0;
word (*BIF_FNS[256])(word, word);

BIF(add) { 
  EVAL_ALL();
  word c = 0;
  // can I put something on the stack and return here 
  // this whole function would just be creating continuations
  while (args != NIL) {
    word car = CONS_CAR(args);
    if (IS_NUM(car)) c += UNREF(car);
    else { printf("Error not a number\n"); exit(1); }
    args = cdr(args);
  }
  return REF_NUM(c);
}

// (fun name (() body))
BIF(fun) {
  word name = FCAR1(args);
  word fargs = FCAR2(args);
  word body = FCDR2(args);
  word doc = NIL;
  if (IS_STR(FCAR1(body))) {
    doc = FCAR1(body);
    body = FCDR1(body);
  }
  word f = fun(name, fargs, body, doc);
  FUN_ENV(f) = env; // closure environment
  return f;
}

BIF(queue) {
  return queue();
}

BIF(sub) {
  EVAL_ALL();
  printf("Sub was called\n");
  return NIL;
}

// add_to_img controls if symbols should be updated for the BIFs
// this allows old images to not get their existing symbols overwritten
// and new images will come with all the built-ins defined as symbols 
// old images could be upgraded and take certain built-ins if they want them
void init_bifs(char add_to_img) {
  DEF_BIF(add, "+");
  DEF_BIF(sub, "-");
  DEF_BIF(fun, "fun");
  DEF_BIF(queue, "queue");
}

word eval_bif(word bif, word args, word env) {
  word i = UNREF(bif);
  if (i >= BIF_LEN) {
    printf("Invalid BIF! You tried to call a BIF that doesn't exist.\n");
    exit(1);
  }
  return BIF_FNS[i](args, env);
}