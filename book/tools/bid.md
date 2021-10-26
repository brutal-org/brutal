# Interface Definition language (BID)

Brutal Interface Definition language or simply BID is a language used for generating C Inter Process Communication code. It is used for communicating with services.

## Definitions

- an `identifier` is every alphabetical character and numeric character (`[a-zA-Z0-9]`).
- a `type` (not the keyword) is a struct, an identifier or an enum (see (#types-in-bid)).

## Type

In bid you have 3 class of types: primitives, structures, and enums.

### The primitive type

A primitive type is only formed of a name:

```bid
identifier
```

You are also able to use generics types using `<` `>`:

```bid
identifier<identifierA, identifierB, identifierC...>
```

So for exemple you may have: `uint64_t`, `BrId`, `Result<int,int>`...

### The structure type

A structure in bid is created using `{` and `}`:

```bid
{
    identifierA: typeA,
    identifierB: typeB,
    ....
}
```

### The enum type

An enum is created when using the  `enum` keyword:

```bid
enum {
    ENTRY_A,
    ENTRY_B,
    ENTRY_C
};
```

## Interface

First each method/structure are placed in an interface, and each interface has a name:

```bid
interface identifier 
{

}
```

For the moment there is only 1 interface supported per file.

An interface may contain:

- `errors`
- `event` (not supported)
- `type`
- `method`
- `id = x`

Each time you add an interface member, they must be separated by a semicolon:

```bid
interface identifier
{
    errors {};
    event {};
    id = 10;
}
```

### Type (keyword)

You can use the keyword `type` for setting a type alias/definition:

```bid
type namea: type;
```

This will generates a typedef or a definition for this type.

### Errors

An `errors` block will contain the different possible errors of the interface.

```bid
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

```bid
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

In BID, a `method` is used to specify an action between a Service and a user app.

The syntax for a method is:

```bid
method identifier arg_type -> return_type;
```

So for example:

```bid
method surface_create {width: int, height: int} -> SurfaceHandle;
```

#### Method C code generation (for the client)

If you have this BID code:

```bid
method surface_create { width: int, height: int } -> { handle: SurfaceHandle, buffer: MObj};
```

BID will generates this:
```c
typedef struct
{
    int width;
    int height;
} CompositorSurfaceCreateRequest;

typedef struct
{
    CompositorSurfaceHandle handle;
    MObj buffer;
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

### Protocol ID

Each interface should contain an unique int identifier:

```bid
id = integral_value
```

so for exemple:

```bid
id = 0x12345
```

Will use the `0x12345` value as an protocol id for the current interface.
