An attempt to be totally image based

Make it readable like a book.
Do minimal compilation (need to be able to read the source).
  - if compile make sure to decompile


(asdf (e 3) (b 1) ie)

(fun asdf ((a 3) (b 1))
  )"# img" 

Python args and kwargs are pretty good
Assign a spot on the stack for each arg and kwarg (regardless if they are passed or not)
Function call 

(<7total|args|4pos> ((a0 a1 a2 a3) (kw0 kw1 kw2)))
(fun asdf (a0 a1 a2 a3)
  asdfoiwjef oi)

(asdf [a 3] [b 1])

(fun asdf (a0 a1 a2 a3)
  asdfoiwjef oi)

This is common lisp: a little confusing...
(defun asdf (a b &key (c 3))
   (list a b c))
(princ (asdf :c 1))

This sort of thing is confusing too:
(defun asdf (&key (a :e) (b :g))
   (list a b))
(princ (asdf :a :v))

This is all special syntax. Do I want special syntax?

Only support alist

(fun asdf (a b)
  )

(fun asdf (a b)
  )

(fun asdf ()
  )

-- expr means you don't evaluate?
(fun asdf ((asdf expr) (asdf expr))
  )

-- this form uses two args so the definition can set that it takes two args
(fun asdf
  (+ ($ 0) ($ 1)))

(block
  (name oiajwef)
  (args a b c) -- map symbols to args control symbol lookup
  a b c)

[asdf (a b c)
 ]

(lambda (x) x)
(fn (a b c) nil)

(asdf 8 7 6 (a 3) (b 2))

(() 1 2)

if the first place is a cons, it takes the right side

((+ ($ 0) ($ 1)) 1 2 3)
 
no such thing as a function

(set a '())

(`(+ 1 ,x) 3)

every code has an invisible 'next'
should i call it "ctx" or "this"?

(add (+ 1 next))

((+ 1 (car this)) 2)

((+ 1 (car this)) )

((if _ 3 2) 2)

first is quoted by default?

(quote
  (if (cont) 2))


eval fun first. check if there is any dispatch with an expr

when you call a expr, gotta pass the args
how to validate number of args

365/365
364/365
363/365
363/365
362/365

(fn b ()
  (fn a (x)
    y))

(fn b ()
  (map 
    (fn (x) (+ x y)) 
    '(1 2 3 4)))



(let
  ((a 1)
   (b 2))
  )