#include <bal/abi.h>
#include <bal/ipc/ev.h>
#include <brutal/debug.h>

WEAK void br_event(MAYBE_UNUSED IpcEv *self, MAYBE_UNUSED BrMsg const *msg)
{
}

void br_ev_handle(IpcEv *self)
{
    BrMsg m = self->ev_arg.msg;
    br_event(self, &m);
}
static void req_dispatch(IpcEv *self)
{
    while (true)
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
                self->ev_arg = ipc;
                fiber_start((FiberFn *)br_ev_handle, self);
            }
        }

        fiber_yield();
    }
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

void br_ev_init(IpcEv *self)
{
    self->dispatcher = fiber_start((FiberFn *)req_dispatch, self);
    self->dispatcher->state = FIBER_IDLE;
}

BrResult br_ev_req(IpcEv *self, BrId to, BrMsg *req, BrMsg *resp)
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

BrResult br_ev_resp(MAYBE_UNUSED IpcEv *self, BrMsg const *req, BrMsg *resp)
{
    resp->seq = req->seq;

    return br_ipc(&(BrIpcArgs){
        .to = req->from,
        .flags = BR_IPC_SEND,
        .deadline = 0,
        .msg = *resp,
    });
}

static bool wait_exit(MAYBE_UNUSED IpcEv *self)
{
    return !self->running;
}

int br_ev_run(IpcEv *self)
{
    self->running = true;

    fiber_block((FiberBlocker){
        .context = nullptr,
        .function = (FiberBlockerFn *)wait_exit,
        .deadline = BR_DEADLINE_INFINITY,
    });

    return self->exit_code;
}

void br_ev_exit(IpcEv *self, int value)
{
    self->exit_code = value;
    self->running = false;
}
