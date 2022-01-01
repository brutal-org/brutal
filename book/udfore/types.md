# Types

## Primitives Types

- Nil
- U8
- U16
- U32
- U64
- I8
- I16
- I32
- I64
- F32
- F64
- Char

## List
Lists are immutable sequences of values

```
T[]
```

Where T is the type.

Lists can be made dynamic by using `:=` when binding them

## Tuple/struct

```
type XYTuple = (x: F64, y: F64);
type XYTuple = (F64, F64);

struct MyXYStruct {
    x : F64,
    y : F64,
};

XYTuple is MyXYStruct; // true

```

## Sum

```
F64 | I64
```

## Box

Represent value stored on the heap

```
type MyBox = [U32]
```

## Object

```
object MyCoolObject : MyCoolBaseObject
{
    x : U32;
    y : U32;
}

```


## Func

Used to represent higher-order functions, meaning that functions can be passed as parameters to other functions

```
InputType -> OutputType

// For example: 
// Func(F32, F32) -> F64

```


## Generic Types

```
type Maybe(T) = T | Nil
```

## User-defined types

Note that types can also define their own values, for example, here is a simple Boolean implementation:

```
type Boolean = True | False
```

## Strong types

```
type MyU32 = U32;

strong type SuperU32 = U32;

MyU32 is U32 // true
MyU32 is SuperU32 // false
SuperU32 is U32 // false
SuperU32 is MyU32 // false
```
