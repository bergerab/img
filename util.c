#include "main.h"

word intern(char *s, word pac) {
  word len = strlen(s);
  word cursor = PAC_SYMS(pac);
  while (cursor != NIL) { 
    word sym = FCAR1(cursor);
    word name = SYM_NAME(sym);
    if (STR_LEN(name) == len && strcmp(STR_CSTR(name), s) == 0)
      return sym;
    cursor = FCDR1(cursor);
  }
  word new_sym = sym(str(s), NIL, pac, NIL);
  PAC_SYMS(pac) = cons(new_sym, PAC_SYMS(pac));
  return new_sym;
}

word car(word cons) { return cons == NIL ? NIL : DEREF(cons); }
word cdr(word cons) { return cons == NIL ? NIL : IMGREF(cons)[1]; }
word count(word cons) {
  word c = 0;
  while (cons != NIL) { cons = cdr(cons); ++c; }
  return c;
}

void show_pac(word pac) {
  word cursor = PAC_SYMS(pac);
  word name = PAC_NAME(pac);
  while (cursor != NIL) { 
    word sym = FCAR1(cursor);
    printf("%s:%s\n", STR_CSTR(name), STR_CSTR(SYM_NAME(sym)));
    cursor = FCDR1(cursor);
  }
}
