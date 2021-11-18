#pragma once

#include <bal/ipc/ev.h>

typedef enum
{
    BID_HANDLER_REQ_RESP,
    BID_HANDLER_NIL_RESP,
    BID_HANDLER_REQ_NIL,
    BID_HANDLER_NIL_NIL,
} BidHandlerType;

typedef struct
{
    BidHandlerType type;
    union
    {
        int (*req_resp)(IpcEv *ev, BrTask task, void *req, void *resp, Alloc *alloc);
        int (*nil_resp)(IpcEv *ev, BrTask task, void *resp, Alloc *alloc);
        int (*req_nil)(IpcEv *ev, BrTask task, void *req);
        int (*nil_nil)(IpcEv *ev, BrTask task);
    };
} BidHandler;

int bid_hook_call(
    IpcEv *self,
    BrId to,
    int proto,
    int req_id, void *req, BalPackFn *req_pack,
    int resp_id, void *resp, BalUnpackFn *req_unpack,
    Alloc *alloc);

void bid_hook_handle(
    BidHandler handler,
    IpcEv *ev,
    BrId from,
    void *req, BrMsg *req_msg, BalUnpackFn *req_unpack,
    void *resp, BrMsg *resp_msg, BalPackFn *resp_unpack,
    Alloc *alloc);
