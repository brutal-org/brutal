#pragma once

#include <bal/abi.h>
#include <brutal/ds.h>
#include <brutal/fibers.h>
#include <ipc/capability.h>
#include <ipc/pack.h>
#include <ipc/unpack.h>

typedef struct _IpcPending IpcPending;
typedef struct _IpcComponent IpcComponent;
typedef struct _IpcBinding IpcBinding;
typedef struct _IpcProvider IpcProvider;

struct _IpcPending
{
    bool ok;
    BrId from;
    uint64_t seq;

    BrMsg resp;
};

typedef void IpcHandler(struct _IpcComponent *ev, BrMsg *req, void *vtable, void *ctx);

struct _IpcProvider
{
    uint64_t port;
    uint64_t proto;
    IpcHandler *handler;
    void *vtable;
    void *ctx;
};

typedef void IpcEventHandler(void *ctx, BrEvent event);

struct _IpcBinding
{
    BrEvent event;
    IpcEventHandler *handler;
    void *ctx;
};

struct _IpcComponent
{
    Alloc *alloc;
    Fiber *dispatcher;
    Vec(IpcPending *) pendings;
    Vec(IpcProvider *) providers;
    Vec(IpcBinding *) bindings;
    Vec(IpcCap) capabilities;
    bool running;
    int result;
};

IpcComponent *ipc_component_self(void);

void ipc_component_init(IpcComponent *self, Alloc *alloc);

void ipc_component_deinit(IpcComponent *self);

void ipc_component_inject(IpcComponent *self, IpcCap const *caps, int count);

Iter ipc_component_query(IpcComponent *self, uint32_t proto, IterFn *iter, void *ctx);

IpcCap ipc_component_require(IpcComponent *self, uint32_t proto);

IpcCap ipc_component_provide(IpcComponent *self, uint32_t id, IpcHandler *fn, void *vtable, void *ctx);

void ipc_component_revoke(IpcComponent *self, IpcCap cap);

void ipc_component_bind(IpcComponent *self, BrEvent event, IpcEventHandler *fn, void *ctx);

void ipc_component_unbind(IpcComponent *self, BrEvent event, void *ctx);

void ipc_component_unbind_all(IpcComponent *self, void *ctx);

BrResult ipc_component_request(IpcComponent *self, IpcCap to, BrMsg *req, BrMsg *resp);

BrResult ipc_component_respond(IpcComponent *self, BrMsg const *req, BrMsg *resp);

int ipc_component_main(IpcComponent *self);

int ipc_component_run(IpcComponent *self);

void ipc_component_exit(IpcComponent *self, int value);
