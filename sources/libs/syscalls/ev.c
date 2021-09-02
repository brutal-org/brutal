#include <brutal/fibers.h>
#include <syscalls/syscalls.h>

typedef struct ipc_job
{
    uint32_t seq;
    bool ok;
    BrMsg *resp;

    struct ipc_job *next;
    struct ipc_job *prev;
} IpcJob;

static Fiber *dispatcher = nullptr;
static IpcJob *jobs = nullptr;

__attribute__((weak)) void br_event(MAYBE_UNUSED BrMsg const *msg)
{
}

static void req_dispatch(void)
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

            for (IpcJob *j = jobs; j; j = j->next)
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
                br_event(&ipc.msg);
            }
        }

        fiber_yield();
    }
}

static void ensure_dispatcher(void)
{
    if (dispatcher == nullptr)
    {
        dispatcher = fiber_start(req_dispatch);
        dispatcher->state = FIBER_IDLE;
    }
}

static void queue_job(IpcJob *job)
{
    if (jobs)
    {
        job->next = jobs->next;
        job->prev = jobs;

        jobs->next->prev = job;
        jobs->next = job;
    }
    else
    {
        jobs = job;
        jobs->next = jobs;
        jobs->prev = jobs;
    }
}

static void dequeue_job(IpcJob *job)
{
    if (job == job->next)
    {
        jobs = nullptr;
    }
    else
    {
        job->prev->next = job->next;
        job->next->prev = job->prev;
    }
}

static bool req_wait(IpcJob *job)
{
    if (!job->ok)
    {
        return false;
    }

    dequeue_job(job);

    return true;
}

BrResult br_ev_req(BrId to, BrMsg *req, BrMsg *resp)
{
    ensure_dispatcher();

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

    queue_job(&job);

    fiber_block((FiberBlocker){
        .function = (FiberBlockerFn *)req_wait,
        .context = (void *)&job,
        .deadline = -1,
    });

    return BR_SUCCESS;
}

BrResult br_ev_resp(BrMsg const *req, BrMsg *resp)
{
    ensure_dispatcher();

    resp->seq = req->seq;

    return br_ipc(&(BrIpcArgs){
        .to = req->from,
        .flags = BR_IPC_SEND,
        .deadline = 0,
        .msg = *resp,
    });
}

static bool running = false;
static int exit = 0;

static bool wait_exit(MAYBE_UNUSED void *context)
{
    return !running;
}

int br_ev_run(void)
{
    ensure_dispatcher();

    running = true;

    fiber_block((FiberBlocker){
        .context = nullptr,
        .function = wait_exit,
        .deadline = BR_DEADLINE_INFINITY,
    });

    return exit;
}

void br_ev_exit(int value)
{
    running = false;
    exit = value;
}
