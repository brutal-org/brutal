# Core features

## Everything is an expression

In Udfore, everything is an expression, meaning that `1;` is a valid udfore file

### Bindings

`let` expressions are called bindings and return their value.

```rs
let a = 10;
```

is an expression that returns 10.

### Functions

Functions return lambdas.

```go
func hello_world(name: String)
```

is, in fact,

```rs
let hello_world = lambda(name: String) {

}
```


## Functional features

Udfore is a multi-paradigm programming language but it has a strong emphasis on functional programming.

Udfore can manipulate functions as data.

doing

```rs
func foo() {}

let a = foo;

a();
``` 

is possible in udfore
