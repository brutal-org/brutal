#pragma once

#include <bal/ipc/ev.h>

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
    BalPackFn *req_pack;
    BalUnpackFn *req_unpack;

    int resp_id;
    BalPackFn *resp_pack;
    BalUnpackFn *resp_unpack;
} IdlBinding;

int idl_hook_call(
    IpcEv *self,
    BrAddr to,
    IdlBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc);

void idl_hook_handle(
    IdlHandler handler,
    IpcEv *ev,
    BrMsg *msg,
    IdlBinding binding,
    void *req,
    void *resp);
