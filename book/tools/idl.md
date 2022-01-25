# Interface Definition language (IDL)

Brutal Interface Definition language or simply IDL is a language used for generating C Inter Process Communication code. It is used for communicating with services.

## Definitions

- an `identifier` is every alphabetical character and numeric character (`[a-zA-Z0-9]`).
- a `type` (not the keyword) is a struct, an identifier or an enum (see (#types-in-idl)).

## Type

In idl you have 3 class of types: primitives, structures, and enums.

### The primitive type

A primitive type is only formed of a name:

```idl
identifier
```

You are also able to use generics types using `<` `>`:

```idl
identifier<identifierA, identifierB, identifierC...>
```

So for exemple you may have: `uint64_t`, `BrId`, `Result<int,int>`...

### The structure type

A structure in idl is created using `{` and `}`:

```idl
{
    identifierA: typeA,
    identifierB: typeB,
    ....
}
```

### The enum type

An enum is created when using the `enum` keyword:

```idl
enum {
    ENTRY_A,
    ENTRY_B,
    ENTRY_C
};
```

## Interface

First each method/structure are placed in an interface, and each interface has a name:

```idl
interface identifier
{

}
```

For the moment there is only 1 interface supported per file.

An interface may contain:

- `errors`
- `type`
- `method`

Each time you add an interface member, they must be separated by a semicolon:

```idl
interface identifier
{
    errors {};
}
```

### Type (keyword)

You can use the keyword `type` for setting a type alias/definition:

```idl
type namea: type;
```

This will generates a typedef or a definition for this type.

### Errors

An `errors` block will contain the different possible errors of the interface.

```idl
errors
{
    ENTRY_A,
    ENTRY_B,
    ...
}
```

It uses the same formation as an enum.

By default:

- `SUCCESS`
- `BAD_COMMUNICATION`
- `UNEXPECTED_MESSAGE`

are always added in error block.

If you have:

```idl
interface Compositor
{
    errors
    {
        UNKNOWN_ACTION,
    };
}

```

You will get:

```c
typedef enum
{
    COMPOSITOR_UNKNOWN_ACTION,
    COMPOSITOR_UNEXPECTED_MESSAGE,
    COMPOSITOR_BAD_COMMUNICATION,
    COMPOSITOR_SUCCESS,
} CompositorError;
```

### Methods

In IDL, a `method` is used to specify an action between a Service and a user app.

The syntax for a method is:

```idl
method identifier arg_type -> return_type;
```

So for example:

```idl
method surface_create {width: int, height: int} -> SurfaceHandle;
```

#### Method C code generation (for the client)

If you have this IDL code:

```idl
method surface_create { width: int, height: int } -> { handle: SurfaceHandle, buf: MObj};
```

IDL will generates this:

```c
typedef struct
{
    int width;
    int height;
} CompositorSurfaceCreateRequest;

typedef struct
{
    CompositorSurfaceHandle handle;
    MObj buf;
} CompositorSurfaceCreateResponse;

typedef enum
{
    COMPOSITOR_INVALID,
    COMPOSITOR_ERROR,
    COMPOSITOR_SURFACE_CREATE_REQUEST,
    COMPOSITOR_SURFACE_CREATE_RESPONSE,
} CompositorMessageType;

static inline CompositorError compositor_surface_create(BrId task, CompositorSurfaceCreateRequest const *req, CompositorSurfaceCreateResponse *resp)
{
    ...
}
```

#### Method C code generation (for the server)

The server (for the same C code as before) will be able to create custom callback for each message type:

```c
typedef struct
{
    void *ctx;
    CompositorSurfaceCreateFn *handle_surface_create;
    CompositorErrorFn *handle_error;
} CompositorServer;

static inline void compositor_server_dispatch(CompositorServer *server, BrMsg const *req_msg)
{
    ...
}
```
