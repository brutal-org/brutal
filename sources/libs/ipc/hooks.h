#pragma once

#include <ipc/component.h>

typedef enum
{
    IDL_HANDLER_REQ_RESP,
    IDL_HANDLER_NIL_RESP,
    IDL_HANDLER_REQ_NIL,
    IDL_HANDLER_NIL_NIL,
} IdlHandlerType;

typedef int IdlHandlerFn(void *ctx, void const *req, void *resp, Alloc *alloc);

typedef struct
{
    IpcProto proto;

    int req_id;
    IpcPackFn *req_pack;
    IpcUnpackFn *req_unpack;

    int resp_id;
    IpcPackFn *resp_pack;
    IpcUnpackFn *resp_unpack;
} IdlBinding;

int ipc_hook_call(
    IpcComponent *self,
    IpcCap to,
    IdlBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc);

void ipc_hook_handle(
    IpcComponent *self,

    IdlHandlerFn handler,
    void *ctx,

    BrMsg *msg,
    IdlBinding binding,

    void *req,
    void *resp);
