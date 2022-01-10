#pragma once

#include <ipc/component.h>

typedef enum
{
    IDL_HANDLER_REQ_RESP,
    IDL_HANDLER_NIL_RESP,
    IDL_HANDLER_REQ_NIL,
    IDL_HANDLER_NIL_NIL,
} IdlHandlerType;

typedef int IdlHandlerFn();

typedef struct
{
    IdlHandlerType type;
    IdlHandlerFn *func;
} IdlHandler;

typedef struct
{
    int proto;

    int req_id;
    IpcPackFn *req_pack;
    IpcUnpackFn *req_unpack;

    int resp_id;
    IpcPackFn *resp_pack;
    IpcUnpackFn *resp_unpack;
} IdlBinding;

int ipc_hook_call(
    IpcComponent *self,
    BrAddr to,
    IdlBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc);

void ipc_hook_handle(
    IdlHandler handler,
    IpcComponent *ev,
    BrMsg *msg,
    IdlBinding binding,
    void *req,
    void *resp);
