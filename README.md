```
An attempt to be totally image based
Maybe compilation is the enemy of a successful image environment.
Whenever you compile something or have a macro, you are fixing the behavior
If the environment is dynamic enough, you will be updating functions and symbols at runtime
We don't want to have to recompile everything if we change one thing.
Another issue is that we want to be able to reproduce the source easily.
if you compile, you need a function to decompile (could be done if you add enough annotation to your compiled code)
Image based systems can be confusing if they don't reliably produce the expected source.
E.g. if you use SSMS to create an existing function, the result is usually a bit different than what you inputted
e.g. SLIME is annoying when your image gets out of sync with the source code

Why not ditch the source code and not rely on files and have the image be the only source of truth.
This has been done before (I think) with smalltalk.


## some idea i had about lisp
the paren syntax denotes a cons list.
but it gets confusing when the (1 . 2) syntax is used. Its infix
I thought it is best to have another type of paren that truely makes a cons
This makes things easier to see e.g. 
([a 1] [b 2] [c 3])
over
((a . 1) (b . 2) (c . 3))
The meaning of (a . 1) is hard to understand on its own. It looks like a cons list but its really not 
the reader translates it to cons(a, 1) (I think?)
square brackets would work the same as "." but it makes it obvious that this is another type of expression that isn't a cons list.
[a [b [c nil]]]
VS
(a . (b . (c . nil)))

Make it readable like a book.

Keeping everything as cons cells works really well.
I made a queue that works great. no more reversing cons lists

Using this image format you can also make arrays with cons cells because they are all allocated in order


I like this image format. very easy to save and load

Builtins are done in a style i like too. function pointers are probably better than doing switch case (could be extended to do calls to dlls)

I'm happy about the string impl. keep count, and keeps cstr. easy to print and has advantage of getting count (dont care about the extra word)

env lookup is powerful but slow -- uses the simple way of a pure list
- closures should work with this


```
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
