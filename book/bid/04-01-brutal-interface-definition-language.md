
# Brutal Interface Definition language

BID is a language used for generating C Inter Process Communication code. It is used for communicating with services.
**This documentation is a simple look at BID, the spec/documentation may change, and may be outdated, or incomplete <!>.**

## What BID looks like ?

First each method/structure are placed in an interface, and each interface has a name:

```bid
interface name 
{

}
```

For the moment there is only 1 interface supported per file.

An interface may contain:

- `errors`
- `event`
- `type`
- `method`
- `id = x`

Each time you add an interface member, they must be separated by a semicolon:

```bid
interface name
{
    errors {};
    event {};
    id = 10;
}
```

### Errors

An `errors` block will contain the different possible errors of the interface.

```bid
errors 
{
    BAD_SIZE,
    BAD_TARGET,
};
```

By default:

- `SUCCESS`
- `BAD_COMMUNICATION`
- `UNEXPECTED_MESSAGE`

are always added in error block.

Note: BID always add the interface name prefix when generating C code, so if you had:

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
method name (arg1: arg1_type, ... ) -> {return1: return1_type, ...};
```

So for example:

```bid
method surface_create (width: int, height: int) -> { handle: SurfaceHandle, buffer: MObj};
```

If you use a custom BID type as an argument / return you can omit the argument name:

```bid

type PingPong : {
    time : Time
};

method ping PingPong -> PingPong;

```

#### C Code (for the client)

If you have this BID code:
```bid
method surface_create (width: int, height: int) -> { handle: SurfaceHandle, buffer: MObj};
```

BID may generate this:
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

#### C code (for the server)

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

It allows to have a simple code for the server and the client.
