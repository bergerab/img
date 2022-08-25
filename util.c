#include "main.h"

word intern(char *s) {

}

word arg_props(word args) {
  // one byte to say how many required arguments (min)
  // two bytes to say how many optional arguments plus required arguments (max)
  // one bit to say if this is supports "&rest"
  word req = 0;
  while (cons != NIL) { 
    if (cons == intern("&rest")) {
      args = cdr(cons);
      
    }
    if (cons == intern("&rest")) {
      args = cdr(cons);
      
    }
    args = cdr(cons); 
    ++req; 
  }
  if (req > 256) { 
    printf("Too many arguments for function. Max is 256.");
    exit(1);
  }
  return req;
}

word car(word cons) { return cons == NIL ? NIL : DEREF(cons); }
word cdr(word cons) { return cons == NIL ? NIL : IMGREF(cons)[1]; }
word count(word cons) {
  word c = 0;
  while (cons != NIL) { cons = cdr(cons); ++c; }
  return c;
}