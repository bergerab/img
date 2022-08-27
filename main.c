#include "main.h"

word *IMG; // image
word LEN; // length of the image
word I; // next index of free memory
word NIL; // reference to NIL
word MAIN; // the root package
word PAC; // the current package
word FREE; // the free list
word VER;

#include "util.c"
#include "bif.c"

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

word sym(word name, word val, word pac, word doc) {
  word s = cons(val, cons(name, cons(pac, cons(doc, cons(NIL, NIL))))); // put the value first so its easy to set
  return REF_SYM(UNREF(s)); // put the name information last so it takes an extra lookup
}

word pac(word name, word pacs, word syms, word doc) { // packages are like folders for symbols
  word p = cons(syms, cons(name, cons(pacs, cons(doc, NIL))));
  return REF_PAC(UNREF(p));
}

void init_img() { /* bootstraps an initial environment */
  VER = 0;
  LEN = 2048;
  IMG = malloc(sizeof(word) * LEN);
  I = 0;
  NIL = sym(str("nil"), 0, 0, 0);
  SYM_VAL(NIL) = NIL; SYM_DOC(NIL) = NIL;
  MAIN = pac(str("main"), NIL, cons(NIL, NIL), str("This is the main package"));
  SYM_PAC(NIL) = MAIN;
  PAC = intern("pac", MAIN);
  FREE = intern("flist", MAIN);
}

void dump_img(char *fp) {
  FILE *f = fopen(fp, "wb");
  /* Need to expose some pointers so we know what is what
     (because the pointer/type trick is being used) */
  char word_size = sizeof(word); char zero = 0;
  fwrite(&word_size, 1, 1, f); fwrite(&zero, word_size - 1, 1, f);
  fwrite(&VER,  sizeof(word), 1, f);
  fwrite(&MAIN, sizeof(word), 1, f);
  fwrite(&NIL,  sizeof(word), 1, f);
  fwrite(&I,    sizeof(word), 1, f);
  fwrite(&LEN,  sizeof(word), 1, f);
  fwrite(IMG,   sizeof(word), I, f);
  fclose(f);
}

void load_img(char *fp) {
  FILE *f = fopen(fp, "rb");
  char word_size, zero;
  fread(&word_size, 1, 1, f); fwrite(&zero, word_size - 1, 1, f);
  fread(&VER,  sizeof(word), 1, f);
  fread(&MAIN, sizeof(word), 1, f);
  fread(&NIL,  sizeof(word), 1, f);
  fread(&I,    sizeof(word), 1, f);
  fread(&LEN,  sizeof(word), 1, f);
  IMG = malloc(sizeof(word) * LEN);
  if (IMG == NULL) { printf("Failed to malloc image."); exit(1); }
  fread(IMG, sizeof(word), I, f);
  fclose(f);
  PAC = intern("pac", MAIN);
}

word eval(word e, word ctx) {
  if (IS_SYM(e)) {
    return SYM_VAL(e);
  } else if (IS_CONS(e)) {
    word car = FCAR1(e);
    word args = FCDR1(e);
    if (IS_SYM(car)) car = eval(car, ctx);
    if (IS_BIF(car)) {
      return eval_bif(car, args, ctx);
    } else if (IS_CONS(car)) {
      return eval(car, args);
    } else {
      printf("Invalid object in car spot.\n");
      exit(1);
    }
  } else {
    return e;
  }
}

void print(word e) {
  if (IS_SYM(e)) printf("%s", STR_CSTR(SYM_NAME(e)));
  else if (IS_NUM(e)) printf("%lld", UNREF(e));
  else if (IS_STR(e)) printf("\"%s\"", STR_CSTR(e));
  else if (IS_PAC(e)) printf("<pac %s>", STR_CSTR(PAC_NAME(e)));
  else if (IS_CONS(e)) {
    printf("IS CONS\n");
    printf("(");
    print(CONS_CAR(e));
    word c = CONS_CDR(e);
    while (c != NIL) {
      if (IS_CONS(c)) {
        printf(" ");
        print(CONS_CAR(c));
        c = CONS_CDR(c);
      } else {
        printf(" ");
        print(c);
        break;
      }
    }
    printf(")");
  }
  else {
    printf("Failed to print unknown type %lld\n", TYPE_CODE(e));
    exit(1);
  }
}

int main () {
  char load = 0;
  if (load) {
    load_img("mydump.img");
    init_bifs(0);
  } else {
    init_img();
    init_bifs(1);
  }
  word r = eval(cons(intern("+", MAIN), cons(REF_NUM(1), cons(REF_NUM(2), cons(REF_NUM(3), NIL)))), NIL);
  print(r);
  //dump_img("mydump.img");
  //show_pac(MAIN);
  return 1;
}