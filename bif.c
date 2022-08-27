#include "main.h"

/* BIF - built-in forms */

#define BIF(cname) word bif_##cname(word args, word ctx)

#define DEF_BIF(cname, name)      \
  BIF_FNS[BIF_LEN] = bif_##cname; \
  if (add_to_img) SYM_VAL(intern(name, MAIN)) = BIF_REF(BIF_LEN++)

word BIF_LEN = 0;
word (*BIF_FNS[256])(word, word);

BIF(add) { 
  word c = 0;
  while (args != NIL) {
    word car = CONS_CAR(args);
    if (IS_NUM(car)) c += UNREF(car);
    else c += UNREF(eval(car, NIL));
    args = cdr(args);
  }
  return REF_NUM(c);
}

BIF(sub) {
  printf("Sub was called\n");
  return NIL;
}

BIF(get_ctx) {
  return ctx;
}

// add_to_img controls if symbols should be updated for the BIFs
// this allows old images to not get their existing symbols overwritten
// and new images will come with all the built-ins defined as symbols 
// old images could be upgraded and take certain built-ins if they want them
void init_bifs(char add_to_img) {
  DEF_BIF(add, "+");
  DEF_BIF(sub, "-");
  DEF_BIF(get_ctx, "ctx");
}

word eval_bif(word bif, word args, word ctx) {
  word i = UNREF(bif);
  if (i >= BIF_LEN) {
    printf("Invalid BIF! You tried to call a BIF that doesn't exist.\n");
    exit(1);
  }
  return BIF_FNS[i](args, ctx);
}