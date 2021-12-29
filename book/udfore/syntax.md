# Syntax

## Variables
Variables are defined with `let`

```
let name: type = value;
```

### To be reviewed:
- Maybe the variable's type could be omitted, as well as `let`?
- Maybe for mutable variables, we could use `:=` instead of `=` (but that's just me memeing)


## Functions
Functions are defined with the `func` keyword, for example:

```
func my_function(param1: type) -> return_type 
{
  return 0;
}
```

### To be reviewed:
- Maybe single-statement functions could be written as `func add_two_nums(param: int, param1: int) = param + param1;` ?
- What about generic functions? `func generic_func(param: T)` like OCaml (without the need of specifying `<T>` or something) ?

## Conditionals
Every test/check returns a boolean, here are the different tests:

- `==`: test if values are equal
- `!=`: test if values are not equal
- `is`: test if types are equal
- `not`: test if types are not equal
- `>=`: greater or equal than
- `<=`: smaller or equal than
- `>`: greater than
- `<`: smaller than
  
### If
If statements are defined with the `if` keyword, like this:
     
```
if (condition)
{
// body
}
```
### Match
Match statements allow for pattern matching, similar to OCaml and Haskell:

```
match x with 
{
  3 -> 4, // If x == 3
  _ -> 3 // Else (similar to default: )
}
```
### To be reviewed:
- Maybe remove `with` in match?

## Disclaimer
This file is just a draft written by a guy that makes weird design choices

