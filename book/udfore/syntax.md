# Syntax

## Variables
Variables are defined with `let`

```rs
let name: type = value;
```

Note that variable types can be inferred, like this:

```rs
let name = value;
```

Mutable variables are defined with `=:` instead of `=`

```rs
let name =: value;
```

## Functions
Functions are defined with the `func` keyword, for example:

```go
func my_function(param1: type) -> return_type 
{
    return 0;
}
```

Note that the return type can also be inferred:

```go
func my_function()
{
    return 0;
}
```

And to simplify things even more, you can define functions with only one expression with `=`:

```go
func return_1() = 1;
```

### Generic functions

Functions can be made generic by adding `@` to the type name.

`@T` here means that we want to have the type of `generic_parameter` stored in `T`

```go
func my_function(generic_parameter: @T) -> T
```

Here, `T` holds the type of `generic_parameter`

## Lambdas
Lambdas are anonymous functions.

```py
lambda (name: type)
{
    return 10;
}
```
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
     
```c
if condition
{
// body
}
```
### Match
Match statements allow for pattern matching, similar to OCaml and Haskell:

```rs
match x 
{
    3 -> 4, // If x == 3
    _ -> 3 // Else (similar to default: )
}
```

## Lists

List is a powerful data type, here are some operations you can do on it:

### Constructing

To construct lists, there are a few operators you can use:

- `::`: cons operator, `1 :: [2,3,4]` is `[1,2,3,4]`
- `++`: concatenate two lists, `[1,2] ++ [3,4]` is `[1,2,3,4]`

### Deconstructing

The cons operator can also be used to deconstruct lists for pattern matching

```rs
match list
{
    head :: tail -> print(head), // Note that to discard the tail, you can do head :: _
}
```

Here, in the `head :: tail` case, we're matching a non-empty list and deconstructing it.

```c++
[1,2,3,4] // 1 :: [2,3,4]
```
