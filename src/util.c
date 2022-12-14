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

// cdr, but frees the cell
word pop(word cons) {
  if (cons == NIL) return NIL;
  word r = FCDR1(cons);
  free_cell(cons);
  return r;
}

// just cons
word push(word c, word val) {
  return cons(val, c);
}

#define Q_HEAD(q) FCAR1(q)
#define Q_TAIL(q) FCAR2(q)
#define Q_SIZE(q) FCAR3(q)

word queue() {
  // first item is head, second is tail, last is count
  return cons(NIL, cons(NIL, cons(REF_NUM(0), NIL)));
}

word queue_last(word q) {
  return car(Q_TAIL(q));
}

word queue_first(word q) {
  return car(Q_HEAD(q));
}

void free_queue(word q) {
  free_list(q);
}

void enqueue(word q, word val) {
  if (Q_HEAD(q) == NIL) Q_HEAD(q) = Q_TAIL(q) = cons(val, NIL);
  else Q_TAIL(q) = FCDR1(Q_TAIL(q)) = cons(val, NIL);
  Q_SIZE(q) = REF_NUM(UNREF(Q_SIZE(q)) + 1);
}

void push_queue(word q, word val) {
  if (Q_HEAD(q) == NIL) Q_HEAD(q) = Q_TAIL(q) = cons(val, NIL);
  else Q_HEAD(q) = cons(val, Q_HEAD(q));
  Q_SIZE(q) = REF_NUM(UNREF(Q_SIZE(q)) + 1);
}

word dequeue(word q) {
  if (Q_HEAD(q) != NIL) {
    word dead = Q_HEAD(q);
    word val = FCAR1(dead);
    Q_HEAD(q) = FCDR1(Q_HEAD(q));
    free_cell(dead);
    Q_SIZE(q) = REF_NUM(UNREF(Q_SIZE(q)) - 1);
    return val;
  }
  return NIL;
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
