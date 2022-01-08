#pragma once

#include <bal/ipc/ev.h>

typedef enum
{
    BID_HANDLER_REQ_RESP,
    BID_HANDLER_NIL_RESP,
    BID_HANDLER_REQ_NIL,
    BID_HANDLER_NIL_NIL,
} BidHandlerType;

typedef int BidHandlerFn();

typedef struct
{
    BidHandlerType type;
    BidHandlerFn *func;
} BidHandler;

typedef struct
{
    int proto;

    int req_id;
    BalPackFn *req_pack;
    BalUnpackFn *req_unpack;

    int resp_id;
    BalPackFn *resp_pack;
    BalUnpackFn *resp_unpack;
} BidBinding;

int bid_hook_call(
    IpcEv *self,
    BrAddr to,
    BidBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc);

void bid_hook_handle(
    BidHandler handler,
    IpcEv *ev,
    BrMsg *msg,
    BidBinding binding,
    void *req,
    void *resp);
