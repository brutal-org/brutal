#include <bal/abi.h>
#include <brutal/debug.h>
#include <ipc/component.h>

typedef struct
{
    IpcComponent *self;
    BrMsg msg;
    IpcHandler *fn;
    void *vtable;
    void *ctx;
} BrEvCtx;

void *br_ev_handle(BrEvCtx *ctx)
{
    BrMsg msg = ctx->msg;
    ctx->fn(ctx->self, &msg, ctx->vtable, ctx->ctx);
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
                (FiberFn *)br_ev_handle,
                &(BrEvCtx){
                    .self = self,
                    .msg = msg,
                    .fn = provider->handler,
                    .vtable = provider->vtable,
                    .ctx = provider->ctx,
                });

            return true;
        }
    }

    return false;
}

static bool dispatch_to_sink(IpcComponent *self, BrMsg msg)
{
    if (!self->sink)
    {
        return false;
    }

    log$("Dispatched message to sink");

    fiber_start(
        (FiberFn *)br_ev_handle,
        &(BrEvCtx){
            .self = self,
            .msg = msg,
            .ctx = nullptr,
            .fn = self->sink,
        });

    return true;
}

static void *ipc_component_dispatch(IpcComponent *self)
{
    do
    {
        BrIpcArgs ipc = {
            .flags = BR_IPC_RECV | BR_IPC_BLOCK,
            .deadline = fiber_deadline(),
        };

        BrResult result = br_ipc(&ipc);

        if (result == BR_SUCCESS)
        {
            if (!(dispatch_to_pending(self, ipc.msg) ||
                  dispatch_to_provider(self, ipc.msg) ||
                  dispatch_to_sink(self, ipc.msg)))
            {
                log$("Unhandled message: {}:{} on port {}", ipc.msg.prot, ipc.msg.type, ipc.msg.to.port);
            }
        }

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
    *self = (IpcComponent){};
    self->alloc = alloc;
    self->running = true;
    self->dispatcher = fiber_start((FiberFn *)ipc_component_dispatch, self);
    self->dispatcher->state = FIBER_IDLE;

    vec_init(&self->pendings, alloc);
    vec_init(&self->providers, alloc);
    vec_init(&self->capabilities, alloc);

    _instance = self;
}

void ipc_component_deinit(IpcComponent *self)
{
    vec_deinit(&self->capabilities);
    vec_deinit(&self->providers);
    vec_deinit(&self->pendings);
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
        .deadline = -1,
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
