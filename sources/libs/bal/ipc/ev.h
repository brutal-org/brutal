#pragma once

#include <bal/abi.h>
#include <bal/ipc/pack.h>
#include <bal/ipc/unpack.h>
#include <brutal/ds.h>
#include <brutal/fibers.h>

typedef struct _IpcJob IpcJob;
typedef struct _IpcEv IpcEv;
typedef struct _IpcProto IpcProto;

struct _IpcJob
{
    uint64_t seq;
    bool ok;
    BrMsg *resp;

    IpcJob *next;
    IpcJob *prev;
};

typedef void IpcFn(struct _IpcEv *ev, BrMsg *req, void *ctx);

struct _IpcProto
{
    uint64_t port;
    uint64_t id;
    IpcFn *fn;
    void *ctx;
};

struct _IpcEv
{
    Fiber *dispatcher;
    void *ctx;
    IpcJob *jobs;
    Vec(IpcProto) protos;
    IpcFn *sink;
    bool running;
    int exit_code;
};

void br_ev_init(IpcEv *self, void *ctx, Alloc *alloc);

void br_ev_deinit(IpcEv *self);

BrAddr br_ev_provide(IpcEv *self, uint32_t id, IpcFn *fn, void *ctx);

BrResult br_ev_req_raw(IpcEv *self, BrAddr to, BrMsg *req, BrMsg *resp);

BrResult br_ev_resp_raw(IpcEv *self, BrMsg const *req, BrMsg *resp);

int br_ev_run(IpcEv *self);

void br_ev_exit(IpcEv *self, int value);
