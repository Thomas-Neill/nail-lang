# nail-lang
"NAIL is a LISP"
NAIL is a small, minimal LISP written in the GNU '99 standard of C.
It's interpreted, untyped, uses symbol literals to proxy references, and still has a few bugs.
It was written as a student project rather than anything of practical use; if I had wanted that, I would've written it in a nice language that doesn't segfault, like Rust or Haskell.

#Syntax/Usage

## Data types
 * Number literals (`123`)
 * String literals (`"Hello world!"`)
 * Symbols (`foo`,`+`,`lambda`)
 * Lists (`(+ 1 2 3)`,`(1 2 3)`)
 * Zilch (`zilch`)
Any of the above can be quoted to defer evaluation.

## Functions (which are also a first-class data-type)
`lambda` and `\` declare anonymous functions.
Constructors for functions take the following arguments:
`(cons-name (arg1 ... argN) codetoevaluate)`
`(def name arglist code)` is equivalent to `(set! name (lambda arglist code))`

### Macros
`macro` declares a macro.
Macros are simply functions that don't evaluate their input.
If you still want to do that, just later, then use `eval!`
`defmacro` is to `macro` as `def` is to `lambda`.

#Standard library
Right now, it's a little undersized, and honestly it probably will remain that way.
We have the common arithmetic operators, and the list trio, `head`, `tail`, and `cons`.
There is also `list`, which constructs a list by evaluating its arguments.
