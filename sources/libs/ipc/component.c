#include <bal/abi.h>
#include <ipc/component.h>
#include <brutal-debug>

typedef struct
{
    IpcComponent *component;
    IpcObject *object;
    IpcProvider provider;
    BrMsg msg;
} BrRequestCtx;

static void *handle_request(BrRequestCtx *ctx)
{
    BrMsg msg = ctx->msg;
    IpcProvider provider = ctx->provider;
    provider.handler(ctx->component, ctx->object, &msg, provider.vtable);
    return nullptr;
}

typedef struct
{
    IpcComponent *self;
    BrMsg msg;
    IpcBinding *binding;
} BrEventCtx;

static void *handle_event(BrEventCtx *ctx)
{
    BrMsg msg = ctx->msg;
    IpcBinding *binding = ctx->binding;
    binding->handler(binding->ctx, msg.event);
    return nullptr;
}

static bool dispatch_to_pending(IpcComponent *self, BrMsg msg)
{
    vec_foreach_v(pending, &self->pendings)
    {
        if (pending->seq == msg.seq && pending->from == msg.from.id)
        {
            pending->resp = msg;
            pending->ok = true;

            return true;
        }
    }

    return false;
}

static bool dispatch_to_object(IpcComponent *self, BrMsg msg)
{
    vec_foreach_v(object, &self->objects)
    {
        if (ipc_object_dispatch(object, self, msg))
        {
            return true;
        }
    }

    return false;
}

static bool dispatch_to_binding(IpcComponent *self, BrMsg msg)
{
    if (br_addr_eq(msg.from, BR_ADDR_EVENT))
    {
        vec_foreach_v(binding, &self->bindings)
        {
            if (br_event_eq(msg.event, binding->event))
            {
                fiber_start_and_forget(
                    (FiberFn *)handle_event,
                    &(BrEventCtx){
                        .self = self,
                        .msg = msg,
                        .binding = binding,
                    });

                return true;
            }
        }
    }

    return false;
}

void ipc_component_pump(IpcComponent *self, BrDeadline deadline)
{
    BrIpcArgs ipc = {
        .flags = BR_IPC_RECV | (deadline > 0 ? BR_IPC_BLOCK : 0),
        .deadline = deadline,
    };

    BrResult result = br_ipc(&ipc);

    if (result == BR_SUCCESS)
    {
        if (!(dispatch_to_pending(self, ipc.msg) ||
              dispatch_to_object(self, ipc.msg) ||
              dispatch_to_binding(self, ipc.msg)))
        {
            log$("Unhandled message: {}:{} on port {}", ipc.msg.prot, ipc.msg.type, ipc.msg.to.port);
        }
    }
}

static void *ipc_component_dispatch(IpcComponent *self)
{
    do
    {
        ipc_component_pump(self, fiber_deadline());
        fiber_yield();
    } while (self->running);

    return nullptr;
}

static IpcComponent *_instance = nullptr;

IpcComponent *ipc_self(void)
{
    assert_not_null(_instance);
    return _instance;
}

void ipc_component_init(IpcComponent *self, Alloc *alloc)
{
    assert_truth(_instance == nullptr);

    *self = (IpcComponent){};
    self->alloc = alloc;
    self->running = true;
    self->dispatcher = fiber_start((FiberFn *)ipc_component_dispatch, self);
    self->dispatcher->state = FIBER_IDLE;

    vec_init(&self->bindings, alloc);
    vec_init(&self->pendings, alloc);
    vec_init(&self->objects, alloc);
    vec_init(&self->consumes, alloc);

    _instance = self;
}

void ipc_component_deinit(IpcComponent *self)
{
    vec_deinit(&self->consumes);
    vec_deinit(&self->objects);
    vec_deinit(&self->pendings);
    vec_deinit(&self->bindings);
}

void ipc_component_inject(IpcComponent *self, IpcCap const *caps, int count)
{
    for (int i = 0; i < count; i++)
    {
        vec_push(&self->consumes, caps[i]);
    }
}

IpcCap ipc_component_require(IpcComponent *self, uint32_t proto)
{
    vec_foreach(cap, &self->consumes)
    {
        if (cap->proto == proto)
        {
            return *cap;
        }
    }

    panic$("No capability found for proto: {}", proto);
}

void ipc_component_attach(IpcComponent *self, IpcObject *obj)
{
    vec_push(&self->objects, obj);
}

void ipc_component_detach(IpcComponent *self, IpcObject *obj)
{
    vec_remove(&self->objects, obj);
}

void ipc_component_bind(IpcComponent *self, BrEvent event, IpcEventHandler *fn, void *ctx)
{
    IpcBinding *binding = alloc_make(self->alloc, IpcBinding);

    binding->event = event;
    binding->handler = fn;
    binding->ctx = ctx;

    br_bind(&(BrBindArgs){
        .event = event,
    });

    vec_push(&self->bindings, binding);
}

void ipc_component_unbind(IpcComponent *self, BrEvent event, void *ctx)
{
    for (int i = 0; i < vec_len(&self->bindings); i++)
    {
        IpcBinding *binding = vec_at(&self->bindings, i);

        if (br_event_eq(binding->event, event) && binding->ctx == ctx)
        {
            br_unbind(&(BrUnbindArgs){
                .event = event,
            });

            vec_splice(&self->bindings, i, 1);
            return;
        }
    }
}

static bool wait_pending(void *ctx)
{
    IpcPending *pending = ctx;
    return pending->ok;
}

BrResult ipc_component_request(IpcComponent *self, IpcCap to, BrMsg *req, BrMsg *resp)
{
    static uint32_t seq = 0;
    req->seq = seq++;

    br_ipc(&(BrIpcArgs){
        .to = to.addr,
        .msg = *req,
        .deadline = 0,
        .flags = BR_IPC_SEND,
    });

    if (resp == nullptr)
    {
        return BR_SUCCESS;
    }

    IpcPending *pending = alloc_make(self->alloc, IpcPending);
    pending->from = to.addr.id;
    pending->seq = req->seq;

    vec_push(&self->pendings, pending);

    fiber_block((FiberBlocker){
        .function = wait_pending,
        .context = pending,
        .deadline = TIME_TIMEOUT_INFINITY,
    });

    *resp = pending->resp;

    vec_remove(&self->pendings, pending);
    alloc_free(self->alloc, pending);

    return BR_SUCCESS;
}

BrResult ipc_component_respond(MAYBE_UNUSED IpcComponent *self, BrMsg const *req, BrMsg *resp)
{
    resp->seq = req->seq;
    resp->prot = req->prot;

    return br_ipc(&(BrIpcArgs){
        .to = req->from,
        .flags = BR_IPC_SEND,
        .deadline = 0,
        .msg = *resp,
    });
}

int ipc_component_run(IpcComponent *self)
{
    fiber_await(self->dispatcher);
    return self->result;
}

void ipc_component_exit(IpcComponent *self, int value)
{
    self->result = value;
    self->running = false;
}

/* --- Object --------------------------------------------------------------- */

void ipc_object_init(IpcObject *self, IpcComponent *component, Alloc *alloc)
{
    static uint64_t port_id = 0;
    self->port = port_id++;

    self->component = component;
    vec_init(&self->providers, alloc);

    ipc_component_attach(component, self);
}

void ipc_object_deinit(IpcObject *self)
{
    ipc_component_detach(self->component, self);

    vec_deinit(&self->providers);
}

IpcCap ipc_object_provide(IpcObject *self, uint32_t proto, IpcHandler *fn, void *vtable)
{
    IpcProvider provider = {};

    provider.proto = proto;
    provider.handler = fn;
    provider.vtable = vtable;

    vec_push(&self->providers, provider);
    return ipc_object_cap(self, proto);
}

bool ipc_object_dispatch(IpcObject *self, IpcComponent *component, BrMsg msg)
{
    if (self->port != msg.to.port)
    {
        return false;
    }

    vec_foreach_v(provider, &self->providers)
    {
        if (provider.proto == msg.prot)
        {
            fiber_start_and_forget(
                (FiberFn *)handle_request,
                &(BrRequestCtx){
                    .component = component,
                    .object = self,
                    .provider = provider,
                    .msg = msg,
                });

            return true;
        }
    }

    return false;
}

IpcCap ipc_object_cap(IpcObject *self, IpcProto proto)
{
    return (IpcCap){
        .proto = proto,
        .addr = (BrAddr){
            .id = bal_self_id(),
            .port = self->port,
        },
    };
}
