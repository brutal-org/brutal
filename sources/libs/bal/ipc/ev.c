#include <bal/abi.h>
#include <bal/ipc/ev.h>
#include <brutal/debug.h>

typedef struct
{
    IpcEv *self;
    BrId from;
    BrMsg *msg;
    void *ctx;

    IpcFn *fn;
} BrEvCtx;

void *br_ev_handle(BrEvCtx *ctx)
{
    BrMsg msg = *ctx->msg;
    ctx->fn(ctx->self, &msg, ctx->ctx);
    return nullptr;
}

static void *req_dispatch(IpcEv *self)
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
            bool message_handeled = false;

            for (IpcJob *j = self->jobs; j; j = j->next)
            {
                if (j->seq == ipc.msg.seq)
                {

                    *j->resp = ipc.msg;
                    j->ok = true;

                    message_handeled = true;
                    break;
                }
            }

            if (!message_handeled)
            {

                vec_foreach_v(proto, &self->protos)
                {
                    if (proto.id == ipc.msg.prot)
                    {

                        fiber_start(
                            (FiberFn *)br_ev_handle,
                            &(BrEvCtx){
                                self,
                                ipc.msg.from,
                                &ipc.msg,
                                proto.ctx,
                                proto.fn,
                            });

                        message_handeled = true;
                    }
                }
            }

            if (!message_handeled && self->sink)
            {

                fiber_start(
                    (FiberFn *)br_ev_handle,
                    &(BrEvCtx){
                        self,
                        ipc.msg.from,
                        &ipc.msg,
                        nullptr,
                        self->sink,
                    });
            }
        }

        fiber_yield();
    } while (self->running);

    return nullptr;
}

static void queue_job(IpcEv *self, IpcJob *job)
{
    if (self->jobs)
    {
        job->next = self->jobs->next;
        job->prev = self->jobs;

        self->jobs->next->prev = job;
        self->jobs->next = job;
    }
    else
    {
        self->jobs = job;
        self->jobs->next = self->jobs;
        self->jobs->prev = self->jobs;
    }
}

static void dequeue_job(IpcEv *self, IpcJob *job)
{
    if (job == job->next)
    {
        self->jobs = nullptr;
    }
    else
    {
        job->prev->next = job->next;
        job->next->prev = job->prev;
    }
}

void br_ev_init(IpcEv *self, void *ctx, Alloc *alloc)
{
    self->ctx = ctx;
    self->running = true;
    self->dispatcher = fiber_start((FiberFn *)req_dispatch, self);
    self->dispatcher->state = FIBER_IDLE;
    vec_init(&self->protos, alloc);
}

void br_ev_deinit(IpcEv *self)
{
    vec_deinit(&self->protos);
}

void br_ev_impl(IpcEv *self, uint32_t id, IpcFn *fn, void *ctx)
{
    IpcProto proto = {id, fn, ctx};
    vec_push(&self->protos, proto);
}

typedef struct
{
    IpcEv *ev;
    IpcJob *job;
} ReqWaitCtx;

static bool req_wait(ReqWaitCtx *ctx)
{
    if (!ctx->job->ok)
    {
        return false;
    }

    dequeue_job(ctx->ev, ctx->job);

    return true;
}

BrResult br_ev_req_raw(IpcEv *self, BrId to, BrMsg *req, BrMsg *resp)
{
    static uint32_t seq = 0;
    req->seq = seq++;

    br_ipc(&(BrIpcArgs){
        .to = to,
        .msg = *req,
        .deadline = 0,
        .flags = BR_IPC_SEND,
    });

    IpcJob job = {};
    job.seq = req->seq;
    job.resp = resp;

    queue_job(self, &job);

    fiber_block((FiberBlocker){
        .function = (FiberBlockerFn *)req_wait,
        .context = (void *)&(ReqWaitCtx){self, &job},
        .deadline = -1,
    });

    return BR_SUCCESS;
}

BrResult br_ev_resp_raw(MAYBE_UNUSED IpcEv *self, BrMsg const *req, BrMsg *resp)
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

int br_ev_run(IpcEv *self)
{
    fiber_await(self->dispatcher);
    return self->exit_code;
}

void br_ev_exit(IpcEv *self, int value)
{
    self->exit_code = value;
    self->running = false;
}
