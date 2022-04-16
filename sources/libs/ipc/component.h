#pragma once

#include <bal/abi.h>
#include <ipc/capability.h>
#include <ipc/pack.h>
#include <ipc/unpack.h>
#include <brutal-ds>
#include <brutal-fibers>

typedef struct _IpcPending IpcPending;
typedef struct _IpcComponent IpcComponent;
typedef struct _IpcBinding IpcBinding;
typedef struct _IpcProvider IpcProvider;
typedef struct _IpcObject IpcObject;

struct _IpcPending
{
    bool ok;
    BrId from;
    uint64_t seq;

    BrMsg resp;
};

typedef void IpcHandler(IpcComponent *ev, IpcObject *self, BrMsg *req, void *vtable);

struct _IpcProvider
{
    uint64_t proto;
    IpcHandler *handler;
    void *vtable;
};

struct _IpcObject
{
    IpcComponent *component;
    uint64_t port;
    Vec(IpcProvider) providers;
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
    Vec(IpcObject *) objects;
    Vec(IpcBinding *) bindings;
    Vec(IpcCap) consumes;
    bool running;
    int result;
};

IpcComponent *ipc_self(void);

void ipc_component_init(IpcComponent *self, Alloc *alloc);

void ipc_component_deinit(IpcComponent *self);

void ipc_component_inject(IpcComponent *self, IpcCap const *caps, int count);

IpcCap ipc_component_require(IpcComponent *self, uint32_t proto);

void ipc_component_attach(IpcComponent *self, IpcObject *obj);

void ipc_component_detach(IpcComponent *self, IpcObject *obj);

void ipc_component_bind(IpcComponent *self, BrEvent event, IpcEventHandler *fn, void *ctx);

void ipc_component_unbind(IpcComponent *self, BrEvent event, void *ctx);

BrResult ipc_component_request(IpcComponent *self, IpcCap to, BrMsg *req, BrMsg *resp);

BrResult ipc_component_respond(IpcComponent *self, BrMsg const *req, BrMsg *resp);

int ipc_component_main(IpcComponent *self);

int ipc_component_run(IpcComponent *self);

void ipc_component_exit(IpcComponent *self, int value);

/* --- Object --------------------------------------------------------------- */

void ipc_object_init(IpcObject *self, IpcComponent *component, Alloc *alloc);

void ipc_object_deinit(IpcObject *self);

IpcCap ipc_object_provide(IpcObject *self, uint32_t proto, IpcHandler *fn, void *vtable);

bool ipc_object_dispatch(IpcObject *self, IpcComponent *component, BrMsg msg);

IpcCap ipc_object_cap(IpcObject *self, IpcProto proto);
