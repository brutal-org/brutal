#include <bal/abi.h>
#include <brutal/debug.h>
#include <ipc/component.h>

typedef struct
{
    IpcComponent *self;
    BrMsg msg;
    IpcProvider *provider;
} BrRequestCtx;

static void *handle_request(BrRequestCtx *ctx)
{
    BrMsg msg = ctx->msg;
    IpcProvider *provider = ctx->provider;
    provider->handler(ctx->self, &msg, provider->vtable, provider->ctx);
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

static bool dispatch_to_provider(IpcComponent *self, BrMsg msg)
{
    vec_foreach_v(provider, &self->providers)
    {
        if (provider->port == msg.to.port &&
            provider->proto == msg.prot)
        {
            fiber_start(
                (FiberFn *)handle_request,
                &(BrRequestCtx){
                    .self = self,
                    .msg = msg,
                    .provider = provider,
                });

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
                fiber_start(
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
              dispatch_to_provider(self, ipc.msg) ||
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

IpcComponent *ipc_component_self(void)
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

    vec_init(&self->pendings, alloc);
    vec_init(&self->providers, alloc);
    vec_init(&self->capabilities, alloc);
    vec_init(&self->bindings, alloc);

    _instance = self;
}

void ipc_component_deinit(IpcComponent *self)
{
    vec_deinit(&self->capabilities);
    vec_deinit(&self->providers);
    vec_deinit(&self->pendings);
    vec_deinit(&self->bindings);
}

void ipc_component_inject(IpcComponent *self, IpcCap const *caps, int count)
{
    for (int i = 0; i < count; i++)
    {
        vec_push(&self->capabilities, caps[i]);
    }
}

Iter ipc_component_query(IpcComponent *self, uint32_t proto, IterFn *iter, void *ctx)
{
    vec_foreach(cap, &self->capabilities)
    {
        if (cap->proto == proto && iter(cap, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

IpcCap ipc_component_require(IpcComponent *self, uint32_t proto)
{
    vec_foreach(cap, &self->capabilities)
    {
        if (cap->proto == proto)
        {
            return *cap;
        }
    }

    panic$("No capability found for proto: {}", proto);
}

IpcCap ipc_component_provide(IpcComponent *self, uint32_t proto, IpcHandler *fn, void *vtable, void *ctx)
{
    static uint64_t port_id = 0;
    IpcProvider *provider = alloc_make(self->alloc, IpcProvider);

    provider->proto = proto;
    provider->port = port_id++;
    provider->handler = fn;
    provider->vtable = vtable;
    provider->ctx = ctx;

    vec_push(&self->providers, provider);

    return (IpcCap){
        .addr = (BrAddr){
            bal_self_id(),
            provider->port,
        },
        .proto = proto,
    };
}

void ipc_component_revoke(IpcComponent *self, IpcCap cap)
{
    for (int i = 0; i < vec_len(&self->providers); i++)
    {
        IpcProvider *provider = vec_at(&self->providers, i);

        if (provider->port == cap.addr.port)
        {
            vec_splice(&self->providers, i, 1);
            return;
        }
    }
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

void ipc_component_unbind_all(IpcComponent *self, void *ctx)
{
    for (int i = 0; i < vec_len(&self->bindings); i++)
    {
        IpcBinding *binding = vec_at(&self->bindings, i);

        if (binding->ctx == ctx)
        {
            br_unbind(&(BrUnbindArgs){
                .event = binding->event,
            });

            vec_splice(&self->bindings, i, 1);
            i--;
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
        .deadline = END_OF_TIME,
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
