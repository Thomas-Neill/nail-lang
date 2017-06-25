# nail-lang
"NAIL is a LISP"
NAIL is a small, minimal LISP written in the GNU '99 standard of C.
It's interpreted, untyped, uses symbol literals to proxy references, and still has a few bugs.
It was written as a student project rather than anything of practical use; if I had wanted that, I would've written it in a nice language that doesn't segfault, like Rust or Haskell.

#Syntax/Usage
```
NAIL> (+ 2 3)
5.000000
NAIL> '(1 2 3)
(1.000000 2.000000 3.000000)
NAIL> (* 3 4 5)
60.000000
NAIL> (set! 'foo "eggs")
zilch
NAIL> foo
"eggs"
NAIL> (print! foo)
eggs
zilch
NAIL> (set! 'id (lambda (a) a))
zilch
NAIL> (id id)
[User-defined function]
NAIL> (set! 'make-acc (lambda (a) (lambda (b) (do (print! a) (set! 'a b)))))
zilch
NAIL> (set! 'echoer (make-acc "hello world"))
zilch
NAIL> (echoer "bye")
hello world
zilch
NAIL> (echoer "hi")
bye
zilch
NAIL> quit
```
