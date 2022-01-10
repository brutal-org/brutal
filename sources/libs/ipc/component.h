#pragma once

#include <bal/abi.h>
#include <brutal/ds.h>
#include <brutal/fibers.h>
#include <ipc/capability.h>
#include <ipc/pack.h>
#include <ipc/unpack.h>

typedef struct _IpcPending IpcPending;
typedef struct _IpcComponent IpcComponent;
typedef struct _IpcProvider IpcProvider;

struct _IpcPending
{
    uint64_t seq;
    bool ok;
    BrMsg resp;
};

typedef void IpcHandler(struct _IpcComponent *ev, BrMsg *req, void *ctx);

struct _IpcProvider
{
    uint64_t port;
    uint64_t proto;
    IpcHandler *handler;
    void *ctx;
};

struct _IpcComponent
{
    Alloc *alloc;
    Fiber *dispatcher;
    void *ctx;
    Vec(IpcPending *) pendings;
    Vec(IpcProvider *) providers;
    IpcHandler *sink;
    bool running;
    int result;
};

void ipc_component_init(IpcComponent *self, void *ctx, Alloc *alloc);

void ipc_component_deinit(IpcComponent *self);

IpcCapability ipc_component_provide(IpcComponent *self, uint32_t id, IpcHandler *fn, void *ctx);

BrResult ipc_component_request(IpcComponent *self, BrAddr to, BrMsg *req, BrMsg *resp);

BrResult ipc_component_respond(IpcComponent *self, BrMsg const *req, BrMsg *resp);

int ipc_component_run(IpcComponent *self);

void ipc_component_exit(IpcComponent *self, int value);
