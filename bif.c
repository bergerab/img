#include "main.h"

/* BIF - built-in forms */

#define BIF(cname) word bif_##cname(word args, word env)

#define DEF_BIF(cname, name)                                      \
  BIF_FNS[BIF_LEN] = bif_##cname;                                 \
  if (add_to_img) SYM_VAL(intern(name, MAIN)) = BIF_REF(BIF_LEN); \
  ++BIF_LEN

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
    else { printf("Error not a number: "); print(car); exit(1); }
    args = cdr(args);
  }
  return REF_NUM(c);
}

BIF(fun) {
  word name = FCAR1(args);
  word fargs;
  word body;
  if (IS_CONS(name)) { // unnamed function
    fargs = name;
    name = NIL;
    body = FCDR1(args);
  } else {
    fargs = FCAR2(args);
    body = FCDR2(args);
  }
  word doc = NIL;
  if (IS_STR(car(body))) {
    doc = FCAR1(body);
    body = FCDR1(body);
  }
  word f = fun(name, fargs, body, doc);
  FUN_ENV(f) = env; // closure environment
  SYM_VAL(name) = f;
  return f;
}

BIF(at_doc) {
  word v = FCAR1(args);
  if (IS_SYM(v)) return SYM_DOC(v); 
  if (IS_FUN(v)) return FUN_DOC(v); 
  return NIL;
}

BIF(queue) {
  return queue();
}

BIF(cons) {
  return cons(FCAR1(args), FCAR2(args));
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
  DEF_BIF(at_doc, "@doc");
  DEF_BIF(cons, ".");
}

word eval_bif(word bif, word args, word env) {
  word i = UNREF(bif);
  if (i >= BIF_LEN) {
    printf("Invalid BIF! You tried to call a BIF that doesn't exist %llu only %llu BIFs.\n", i, BIF_LEN);
    exit(1);
  }
  return BIF_FNS[i](args, env);
}