# Coding Style

> Note: coding styles may change over time.
> Please let us know if a piece of code is not compatible with these coding styles <3 !

## Naming

### IDL

- `snake_case` for all methods
- `CONST_CASE` for all constants
- `PascalCase` for all types

### C

- `snake_case` for all identifier
- `CONST_CASE` for all constants
- `PascalCase` for all types

These rules don't apply to standard headers.

Try to use 'namespace' for declaration names, for example:

```c
/* in the file foo.h */
typedef int FooBar;

void foo_init(FooBar *foo);
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

- Use C macros as a function only when it is needed.
- Add a dollar at the end of the macro name when it behaves like a function: `macro$(10, int)`

## Type
- Use the `const` attribute after the type name:
  - do: `int const *` don't: `const int *`
- When you are using a templated datastructure (for example `vec(T)`) for returning or as an argument in a function  Always typedef it before:

Do:
```c
typedef Vec(Foo) FooTable;

FooTable func(void);
```
Don't:
```c
Vec(Foo) func(void);
```

## Function

In C, functions declaration must follow these rules:

- If the function declaration is not in the header, (and not used anywhere else). It should use the `static` attribute.

### Function Arguments
- You should set the `const` keyword to an argument when it is possible.
- Function without any argument must use `void` instead:
    - Do: `void empty(void)` not: `void empty()`.
- Function that behaves like class member must have the first argument as `type (const) * self`:

```c
void object_resize(Object* self, Size new_size);
```

- Function that needs an alloc object must put it at the last argument:

```c
void object_foo(Object* self, Str b, Alloc* alloc);
```

- If an argument is unused during the function execution you must put the `MAYBE_UNUSED` attribute:
```c
int deinit(MAYBE_UNUSED void *data)
{
    // ...
}
```
### Main function

- Always put `argc` and `argv` as arguments.
- Use a double pointer for `argv`.

## Statement

Each statement must put a `{` and a `}` on a new line.


For example:

```c
if (cond)
{
    // ...
}
else
{
    // ...
}

while (cond)
{
    // ...
}

do
{
    // ...
} while (cond);

struct foo_bar
{
};

union foo_bar
{

};

void do_foo()
{
    // ...
}
```

### Keywords

- You must use a space after `if`, `for`, `while`, `switch`...
- You must use parenthesis after a `sizeof`.
- You must use `nullptr` instead of `NULL` and `(Type*)0`.
- You must use curly braces after a statement.

### Switch

You should not put an additional indentation in the switch statement block.
Every switch statement must have a default block, with at least a panic in case you should not land here.
For a switch case, you must use `{}` when the code in the block start to get big.
For example:

```c
switch(case)
{
case 1:
{
    doa();
    dob();
    if(is_c)
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

### Returning In An Else Statement

You must avoid returning in an else statement if no other action can be executed after the else.

For example, you should avoid doing:
```c
int func(void)
{
    if(x)
    {
        return 0;
    }
    else
    {
        return 10;
    }
}
```

And do:
```c
int func(void)
{
    if(x)
    {
        return 0;
    }

    return 10;
}
```


## Formatting

* You can use the command `clang-format -r` for formatting your code automatically

