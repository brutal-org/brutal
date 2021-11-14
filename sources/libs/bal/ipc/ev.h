#pragma once

#include <bal/abi.h>
#include <brutal/fibers.h>

typedef struct ipc_job
{
    uint32_t seq;
    bool ok;
    BrMsg *resp;

    struct ipc_job *next;
    struct ipc_job *prev;
} IpcJob;

typedef struct
{
    Fiber *dispatcher;
    IpcJob *jobs;
    BrIpcArgs ev_arg;
    bool running;
    int exit_code;
} IpcEv;

void br_ev_init(IpcEv *self);

BrResult br_ev_req(IpcEv *self, BrId to, BrMsg *req, BrMsg *resp);

BrResult br_ev_resp(IpcEv *self, BrMsg const *req, BrMsg *resp);

int br_ev_run(IpcEv *self);

void br_ev_exit(IpcEv *self, int value);
