#include "main.h"

word *IMG; // image
word LEN; // length of the image
word I; // next index of free memory
word NIL; // reference to NIL
word *MAIN; // the root package

word SYM_REST;

word cons(word car, word cdr) {
  word cons = REF_CONS(I);
  IMG[I] = car; IMG[I+1] = cdr;
  I += 2;
  return cons;
}

word str(char *s) {
  word str = REF_STR(I);
  IMG[I] = strlen(s);
  strcpy((char*)&IMG[I+1], s); // strings are both length prefixed and are null terminated keeps interop with C that way and who cares about one byte!
  word size = 1 + ceil((IMG[I] + 1)/sizeof(word));
  if (size % 2 == 0) I += size;
  else I += size + 1; // add an extra 1 so when this is GC'd, it could be collected and reused for cons cells (even number)
  return str;
}

word sym(word name, word val, word fun, word pac, word doc) { // value, function value, package
  word s = cons(val, cons(fun, cons(name, cons(pac, cons(doc, cons(NIL, NIL)))))); // put the value first so its easy to set
  return REF_SYM(UNREF(s)); // put the name information last so it takes an extra lookup
}

word fun(word sym, word args, word body, word doc) {
  word fun = cons(arg_props(args), cons(body, cons(args, cons(sym, cons(doc, NIL)))));
  return REF_FUN(UNREF(fun));
}

word pac(word name, word pacs, word syms, word doc) { // packages are like folders for symbols
  word p = cons(syms, cons(name, cons(pacs, cons(doc, NIL))));
  return REF_PAC(UNREF(p));
}

// need FREE list
// need STRING list
// need root pac
// write intern
void init_img() {
  IMG = malloc(sizeof(word) * 2048);
  I = 0;
  LEN = 0;
  NIL = sym(str("nil"), 0, 0, 0, 0);

  SYM_REST = intern("@rest");
}

int main () {
  init_img();
  word s = str("oiajwef");
  printf("%d %s", DEREF(s), STR_CSTR(IMGREF(s)));
  return 1;
}