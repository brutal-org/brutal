# Coding Style

For low-level styling (spaces, parentheses, brace placement, etc), all code should follow the format specified in .clang-format in the project root.

## Naming

### IDL

- `snake_case` for all methods
- `CONST_CASE` for all constants
- `PascalCase` for all types

### C

- `snake_case` for all identifier
- `CONST_CASE` for all constants
- `PascalCase` for all types
- `_PascalCase` for all struct/union/enum

These rules don't apply to standard headers.

Try to use 'namespace' for declaration names, for example:

```c
// foo/bar.h
#pragma once

typedef int FooBar;

void foo_bar_init(FooBar *self);
```

### Json

- `snake_case` for all keys

### Abbreviations

| Full Word                     | Abbreviations |
| ----------------------------- | ------------- |
| allocator, allocation         | alloc         |
| buffer                        | buf           |
| datastructure                 | ds            |
| graphics                      | gfx           |
| interface definition language | idl           |
| initialize                    | init          |
| length                        | len           |
| matrix                        | mat           |
| rasterizer                    | rast          |
| vector                        | vec           |


## Macro

Constant macros should be `UPPER_CASE`

```c
#define FOO_BAR (1)
```

Macros that looks like functions call should be lower_case and ending with `$`

```c
#define foo_bar$(x) x
```

## Type

Prefer east const

```c
// do

int const* foo;

// don't

const int *foo;
```

When you are using a templated data type (e.g. `vec(T)`) for returning or as an argument in a function  Always typedef it before:


```c
// do

typedef Vec(Foo) FooTable;

FooTable func(void);

// Don't

Vec(Foo) func(void);
```

## Symbols

All symbols that are not exposed to the outside world should be marked with `static`

### Functions

Parameters should be marked with `const` by default.

> In C functions without any parameters should be (void)

Function should be namspaced with the type they operate on.

```c
void object_resize(Object* self, size_t const new_size);

void foo_bar(Foo* self, int const x);
```

Function that needs an `Alloc` object must put it at the last argument:

```c
void object_foo(Object* self, Str const b, Alloc* alloc);
```

If an parameters is unused you must put the `MAYBE_UNUSED` attribute

```c
int deinit(MAYBE_UNUSED void *data)
{
    // ...
}
```
### Main function

Always put `argc` and `argv` as arguments, use `char const*[]` for `argv`.

```c
int main(int argc, char const *argv[])
{
    // ...
}
```

### Switch

You should not put an additional indentation in the switch statement block.
Every switch statement must have a default block, with at least a panic in case you should not land here.
For a switch case, you must use `{}` when the code in the block start to get big.

```c
switch(case)
{

case 1:
{
    int a = 10;

    doa();
    dob();

    if(a)
    {
        do_d();
    }

    break;
}

case 2:
    dob();
    break;

default:
    break;
}
```

### If/Else Statement

Use guard clauses when relevant

```c
// Do
int func(void)
{
    if (!x)
    {
        return 0;
    }

    // More logic here...

    return 10;
}

// Don't
int func(void)
{
    if (x)
    {
        // More logic here...

        return 10;
    }
    else
    {
        return 0;
    }
}
```
