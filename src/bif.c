#include "main.h"

/* BIF - built-in forms */

#define BIF(cname) word bif_##cname(word args, word env)

#define DEF_BIF(cname, name)                                      \
  BIF_FNS[BIF_LEN] = bif_##cname;                                 \
  if (add_to_img) SYM_VAL(intern(name, MAIN)) = BIF_REF(BIF_LEN); \
  ++BIF_LEN

#define EVAL_ALL()                         \
  word q = queue();                        \
  while (args != NIL) {                    \
    enqueue(q, eval(CONS_CAR(args), env)); \
    args = CONS_CDR(args);                 \
  }                                        \
  args = Q_HEAD(q);                        \
  free_queue(q)

word BIF_LEN = 0;
word (*BIF_FNS[256])(word, word);

BIF(add) { 
  EVAL_ALL();
  word c = 0;
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
  word f = fun(name, fargs, body, doc, args);
  FUN_ENV(f) = env; // closure environment
  SYM_VAL(name) = f;
  return f;
}

BIF(list) {
  EVAL_ALL();
  return args;
}

BIF(quote) {
  if (FCDR1(args) == NIL) return FCAR1(args);
  return args;
}

BIF(at_doc) {
  word v = FCAR1(args);
  if (IS_SYM(v)) return SYM_DOC(v); 
  if (IS_FUN(v)) return FUN_DOC(v); 
  return NIL;
}

BIF(at_src) {
  return FUN_SRC(FCAR1(args));
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
  DEF_BIF(at_src, "@src");
  DEF_BIF(list, "list");
  DEF_BIF(cons, "cons");
  DEF_BIF(quote, "'");
}

word eval_bif(word bif, word args, word env) {
  word i = UNREF(bif);
  if (i >= BIF_LEN) {
    printf("Invalid BIF! You tried to call a BIF that doesn't exist %llu only %llu BIFs.\n", i, BIF_LEN);
    exit(1);
  }
  return BIF_FNS[i](args, env);
}
