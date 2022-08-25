#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// could allocate everything on its own then make the image later 
// then loading the image and saving would be expensive
// or could keep expanding the same memory and use references into that
// that might be nice. could use the cons trick to make larger cons cells
//
// strings should all be linked together and immutable
//
// image portability
// do I care about being able to support the same image on 32-bit vs 64-bit?
// maybe if we want images to last forever

typedef uint64_t word;

extern word *IMG; // image
extern word LEN; // length of the image
extern word I; // next index of free memory
extern word NIL; // reference to NIL
extern word *MAIN; // the root package

extern word *SYM_REST;

// Fast car -- used when you want to get a car that you know is a few cons deep
// Fast car is also a dangerous car. Use only if you know the cons list is that deep
#define FCAR1(o) (o[0])
#define FCAR2(o) (DEREF(o[1])[0])
#define FCAR3(o) (DEREF(DEREF(o[1])[1])[0])
#define FCAR4(o) (DEREF(DEREF(DEREF(DEREF(o[1])[1])[1])[1])[0])
#define FCAR5(o) (DEREF(DEREF(DEREF(DEREF(DEREF(o[1])[1])[1])[1])[1])[0])
#define FCAR6(o) (DEREF(DEREF(DEREF(DEREF(DEREF(DEREF(o[1])[1])[1])[1])[1])[1])[0])

// using my own IMG instead of actual memory allows us to use
// the pointer hack. It isn't supported usually.
#define REF_NUM(n) (n << 8)
#define UNREF(ref) (ref >> 8)
#define DEREF(ref) (IMG[UNREF(ref)])
#define IMGREF(ref) (&DEREF(ref))
#define REF(ptr, id) ((ptr << 8) | id)
#define IS_TYPE(ptr, id) ((ptr & 8) == id)

// 0000 0001
#define REF_CONS(ptr)  REF(ptr, 1)
#define IS_CONS(ptr)   IS_TYPE(ptr, 1)
#define CONS_CAR(cons) (cons[0])
#define CONS_CDR(cons) (cons[1])
word cons(word car, word cdr);

// 0000 0011
#define REF_STR(ptr)  REF(ptr, 3)
#define IS_STR(ptr)   IS_TYPE(ptr, 3)
#define STR_LEN(str)  (str[0])
#define STR_CSTR(str) ((char*)&str[1])
word str(char *s);

// 0000 0101
#define REF_SYM(ptr)  REF(ptr, 5)
#define IS_SYM(ptr)   IS_TYPE(ptr, 5)
#define SYM_VAL(sym)  FCAR1(sym)
#define SYM_FUN(sym)  FCAR2(sym)
#define SYM_NAME(sym) FCAR3(sym)
#define SYM_PAC(sym)  FCAR4(sym)
#define SYM_DOC(sym)  FCAR5(sym)
word sym(word name, word val, word fun, word pac, word doc); // value, function value, package

// 0000 0111
#define REF_FUN(ptr)  REF(ptr, 7)
#define IS_FUN(ptr)   IS_TYPE(ptr, 7)
#define FUN_BODY(fun) FCAR1(fun)
#define FUN_ARGS(fun) FCAR2(fun)
#define FUN_SYM(fun)  FCAR3(fun)
#define FUN_DOC(fun)  FCAR4(fun)
word fun(word sym, word args, word body, word doc);

// 0000 1011
#define REF_PAC(ptr)  REF(ptr, 11)
#define IS_PAC(ptr)   IS_TYPE(ptr, 11)
#define PAC_SYMS(pac) FCAR1(pac)
#define PAC_NAME(pac) FCAR2(pac)
#define PAC_PACS(pac) FCAR3(pac)
#define PAC_DOC(pac)  FCAR4(pac)
word pac(word name, word pacs, word syms, word doc); // packages are like folders for symbols