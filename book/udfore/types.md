# Types

## Primitives Types

- Nil
- Any
- Never
- Bool
- U8
- U16
- U32
- U64
- I8
- I16
- I32
- I64
- USize
- ISize
- F32
- F64


## Vector

```
U32[3]
```

## Dynamic Vector

Values can be pushed and poped inserted and removed

```
U32[]
```

## Tuple/struct

```
type XYTuple = (x : F64, y : F64);
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

```
InputType -> OutputType
```


## Generic Types

```
type Maybe(T) = T | Nil
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
