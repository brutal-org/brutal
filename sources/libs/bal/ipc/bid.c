#include <bal/hw.h>
#include <bal/ipc/bid.h>
#include <brutal/debug.h>

int bid_hook_call(
    IpcEv *self,
    BrId to,
    int proto,
    int req_id, void const *req, BalPackFn *req_pack,
    int resp_id, void *resp, BalUnpackFn *req_unpack,
    Alloc *alloc)
{

    BrMsg resp_msg = {};
    BrMsg req_msg = {};

    // Packing

    req_msg.prot = proto;
    req_msg.type = req_id;

    BalPack pack;
    bal_pack_init(&pack);

    if (req != nullptr)
    {
        req_pack(&pack, req);
        req_msg.args[0] = pack.obj;
        req_msg.flags = BR_MSG_HND(0);
    }

    br_ev_req_raw(self, to, &req_msg, &resp_msg);

    // Error handeling

    if (resp_msg.type == BR_MSG_ERROR)
    {
        return resp_msg.args[0];
    }

    // Unpacking

    assert_equal(resp_msg.args[0], (BrArg)resp_id);

    if (resp != nullptr)
    {
        BalShm shm;
        balshm_init_mobj(&shm, resp_msg.args[0]);

        BalUnpack unpack;
        bal_unpack_init(&unpack, shm.buf, shm.len, alloc);
        req_unpack(&unpack, resp);
        balshm_deinit(&shm);
    }

    bal_pack_deinit(&pack);

    return 0;
}

static int bid_handle_invoke(BidHandler handler, IpcEv *ev, BrTask from, void *req, void *resp, Alloc *alloc)
{
    switch (handler.type)
    {
    case BID_HANDLER_REQ_RESP:
        return handler.func(ev, from, req, resp, alloc);
    case BID_HANDLER_NIL_RESP:
        return handler.func(ev, from, resp, alloc);
    case BID_HANDLER_REQ_NIL:
        return handler.func(ev, from, req);
    case BID_HANDLER_NIL_NIL:
        return handler.func(ev, from);
    default:
        panic$("Invalid handler type {}", handler.type);
    }
}

void bid_hook_handle(
    BidHandler handler,
    IpcEv *ev,
    BrMsg *msg,

    void *req,
    BalUnpackFn *req_unpack,

    void *resp,
    int resp_id,
    BalPackFn *resp_pack)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    if (req != nullptr)
    {
        BalShm shm;
        balshm_init_mobj(&shm, msg->args[0]);

        BalUnpack unpack;
        bal_unpack_init(&unpack, shm.buf, shm.len, base$(&heap));
        req_unpack(&unpack, req);

        balshm_deinit(&shm);
    }

    int result = bid_handle_invoke(
        handler, ev, msg->from, req, resp, base$(&heap));

    if (result != 0)
    {
        BrMsg resp_msg;
        resp_msg.type = BR_MSG_ERROR;
        resp_msg.args[0] = result;
        br_ev_resp_raw(ev, msg, &resp_msg);
    }
    else if (resp != nullptr)
    {
        BalPack pack;
        bal_pack_init(&pack);

        resp_pack(&pack, resp);

        BrMsg resp_msg;
        resp_msg.type = resp_id;
        resp_msg.args[0] = pack.obj;
        br_ev_resp_raw(ev, msg, &resp_msg);

        bal_pack_deinit(&pack);
    }

    heap_alloc_deinit(&heap);
}
