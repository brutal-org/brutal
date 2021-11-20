#pragma once

#include <bal/abi.h>
#include <bal/ipc/pack.h>
#include <bal/ipc/unpack.h>
#include <brutal/ds.h>
#include <brutal/fibers.h>

typedef struct ipc_job IpcJob;
typedef struct ipc_ev IpcEv;
typedef struct ipc_proto IpcProto;

struct ipc_job
{
    uint32_t seq;
    bool ok;
    BrMsg *resp;

    struct ipc_job *next;
    struct ipc_job *prev;
};

typedef void IpcFn(struct ipc_ev *ev, BrMsg *req, void *ctx);

struct ipc_proto
{
    uint32_t id;
    IpcFn *fn;
    void *ctx;
};

struct ipc_ev
{
    Fiber *dispatcher;
    IpcJob *jobs;
    Vec(IpcProto) protos;
    IpcFn *sink;
    bool running;
    int exit_code;
};

void br_ev_init(IpcEv *self, Alloc *alloc);

void br_ev_deinit(IpcEv *self);

void br_ev_impl(IpcEv *self, uint32_t id, IpcFn *fn, void *ctx);

BrResult br_ev_req_raw(IpcEv *self, BrId to, BrMsg *req, BrMsg *resp);

BrResult br_ev_resp_raw(IpcEv *self, BrMsg const *req, BrMsg *resp);

int br_ev_run(IpcEv *self);

void br_ev_exit(IpcEv *self, int value);
