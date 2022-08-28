#include "main.h"

word *IMG; // image
word LEN; // length of the image
word I; // next index of free memory
word NIL; // reference to NIL
word MAIN; // the root package
word PAC; // the current package
word FREE; // the free list
word VER; // version (for image files)
word CTXS; // contexts
word ARGS; // argument stack
word FUN; // fun symbol
word IF; // if symbol
word POPEN; // ( symbol
word PCLOSE; // ) symbol

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

word fun(word name, word args, word body, word doc) { // packages are like folders for symbols
  word props = REF_NUM(count(args));
  // after body is the ENV this is for closures -- only useful when there's nested functions
  word f = cons(name, cons(props, cons(args, cons(body, cons(NIL, cons(doc, NIL))))));
  return REF_FUN(UNREF(f));
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
  CTXS = intern("ctxs", MAIN);
  ARGS = intern("args", MAIN);
  FUN = intern("fun", MAIN);
  IF = intern("if", MAIN);
  POPEN = intern("(", MAIN);
  PCLOSE = intern(")", MAIN);
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
  CTXS = intern("ctxs", MAIN);
  ARGS = intern("args", MAIN);
  FUN = intern("fun", MAIN);
  IF = intern("if", MAIN);
  POPEN = intern("(", MAIN);
  PCLOSE = intern(")", MAIN);
}

void free_cell(word cons) {

}

void free_list(word cons) {

}

// (fun a (a) "mydoc" (+ a a))
// (a 10) fails
// some numbers are cursed like 0 9 10 100
word eval(word e, word env) {
  if (IS_SYM(e)) {
    while (env != NIL) {
      word pair = FCAR1(env);
      if (FCAR1(pair) == e) return FCDR1(pair);
      env = FCDR1(env);
    }
    return SYM_VAL(e);
  } else if (IS_CONS(e)) {
    word car = FCAR1(e);
    word args = FCDR1(e);

    if (IS_SYM(car) || IS_CONS(car)) car = eval(car, env);
    // argcount, args, and temps
    if (IS_BIF(car)) {
      return eval_bif(car, args, env);
    } else if (IS_FUN(car)) {
      word count = 0;
      word fargs = FUN_ARGS(car);
      while (args != NIL && fargs != NIL) {
        env = cons(cons(FCAR1(fargs), eval(FCAR1(args), env)), env);
        fargs = FCDR1(fargs);
        args = FCDR1(args);
        ++count;
      }
      if (UNREF(FUN_PROPS(car)) != count) {
        printf("Invalid arguments passed. Expected %llu but got %llu.\n", UNREF(FUN_PROPS(car)), count);
        exit(1);
      }
      word body = FUN_BODY(car);
      word r;
      while (body != NIL) {
        r = eval(FCAR1(body), env);
        body = FCDR1(body);
      }
      return r;
    } else {
      printf("Invalid object in car spot %lld.\n", TYPE_CODE(car));
      exit(1);
    }
  } else { // self evaluating forms
    return e;
  }
}

void print_cons(word e) {
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

void print(word e) {
  if (IS_SYM(e))
    printf("%s", STR_CSTR(SYM_NAME(e)));
  else if (IS_NUM(e))
    printf("%lld", UNREF(e));
  else if (IS_STR(e))
    printf("\"%s\"", STR_CSTR(e));
  else if (IS_PAC(e))
    printf("<pac %s>", STR_CSTR(PAC_NAME(e)));
  else if (IS_FUN(e))
    printf("<fun %s>", STR_CSTR(SYM_NAME(FUN_NAME(e))));
  else if (IS_CONS(e)) {
    print_cons(e);
  } else {
    printf("Failed to print unknown type %lld\n", TYPE_CODE(e));
    exit(1);
  }
}

// reader
char is_whitespace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
char is_breakchar(int c) { return c == '(' || c == ')' || c == '"'; }
char is_digit(int c) { return c >= '0' && c <= '9'; }
char skip_whitespace(FILE *f) {
  int c;
  while ((c = fgetc(f)) != EOF && is_whitespace(c));
  if (c == EOF) return 0;
  ungetc(c, f);
  return 1;
}

word read_sym(FILE *f) {
  char s[256]; word i = 0; char is_num = 1; int c;
  if (!skip_whitespace(f)) return 0;
  c = fgetc(f);
  if (c == '"') {
    while ((c = fgetc(f)) != EOF && c != '"' && i < 255) s[i++] = c;
    s[i] = '\0';
    return str(s); // TODO: intern
  } else if (is_breakchar(c)) {
    s[0] = c; s[1] = '\0';
    return intern(s, MAIN);
  }
  ungetc(c, f);
  while ((c = fgetc(f)) != EOF && !is_whitespace(c) && i < 255) {
    if (i > 0 && is_breakchar(c)) { ungetc(c, f); break; }
    s[i++] = c;
    is_num = is_num && is_digit(c);
  }
  if (i == 0) return 0;
  s[i] = '\0';
  if (is_num) return REF_NUM(atoi(s));
  return intern(s, MAIN);
}

word read(FILE *f) { // reads a single expr
  word e, q = NIL;
  if (!skip_whitespace(f)) return 0;
  while ((e = read_sym(f))) {
    if (e == POPEN) {
      if (q == NIL) q = queue();
      push_queue(q, queue());
    } else if (e == PCLOSE) {
      if (UNREF(Q_SIZE(q)) == 1) { // last item ((1 2 3)) -> (1 2 3)
        word sexpr_q = queue_first(q);
        free_queue(q);
        word sexpr = Q_HEAD(sexpr_q);
        free_queue(sexpr_q);
        return sexpr;
      } else { // merge - ((x y z) (a b c) (1 2 3)) -> (1 2 3 (a b c (x y z)))
        word sexpr_q = dequeue(q);
        word sexpr = Q_HEAD(sexpr_q);
        free_queue(sexpr_q);
        enqueue(queue_first(q), sexpr);
      }
    } else {
      if (q == NIL) return e;
      enqueue(queue_first(q), e);
    }
  }
  return Q_HEAD(q);
}

int main() {
  char load = 0;
  if (load) {
    load_img("mydump.img");
    init_bifs(0);
  } else {
    init_img();
    init_bifs(1);
  }

  while (1) {
    printf(">> ");
    word e = read(stdin);
    print(eval(e, NIL));
    printf("\n");
  }

  word r =
      eval(cons(intern("+", MAIN),
                cons(REF_NUM(1), cons(REF_NUM(2), cons(REF_NUM(3), NIL)))), NIL);
  print(r);
  // dump_img("mydump.img");
  // show_pac(MAIN);
  return 1;
}