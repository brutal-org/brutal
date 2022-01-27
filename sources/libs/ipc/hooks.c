#include <bal/hw.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <ipc/hooks.h>

int ipc_hook_call(
    IpcComponent *self,
    IpcCap to,
    IdlBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc)
{
    BrMsg resp_msg = {};
    BrMsg req_msg = {};

    // Packing

    req_msg.prot = binding.proto;
    req_msg.type = binding.req_id;

    IpcPack pack;
    ipc_pack_init(&pack);

    if (req != nullptr)
    {
        binding.req_pack(&pack, req);

        req_msg.args[0] = pack.handle;
        req_msg.flags = BR_MSG_HND(0);

        for (int i = 0; i < pack.handles_count; i++)
        {
            req_msg.args[i + 1] = pack.handles[i];
            req_msg.flags |= BR_MSG_HND(i + 1);
        }
    }

    ipc_component_request(self, to, &req_msg, &resp_msg);

    // Error handeling

    if (resp_msg.type == BR_MSG_ERROR)
    {
        return resp_msg.args[0];
    }

    // Unpacking

    if (resp != nullptr)
    {
        assert_equal(resp_msg.type, (BrArg)binding.resp_id);

        BalMem shm;
        bal_mem_init_mobj(&shm, resp_msg.args[0]);

        IpcUnpack unpack;
        ipc_unpack_init(&unpack, shm.buf, shm.len, alloc);
        binding.resp_unpack(&unpack, resp);
        bal_mem_deinit(&shm);
    }

    ipc_pack_deinit(&pack);

    return 0;
}

void ipc_hook_handle(
    IpcComponent *self,

    IdlHandlerFn handler,
    void *ctx,

    BrMsg *msg,
    IdlBinding binding,

    void *req,
    void *resp)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    if (req != nullptr)
    {
        BalMem shm;
        bal_mem_init_mobj(&shm, msg->args[0]);

        IpcUnpack unpack;
        ipc_unpack_init(&unpack, shm.buf, shm.len, base$(&heap));
        binding.req_unpack(&unpack, req);

        bal_mem_deinit(&shm);
    }

    int result = handler(ctx, req, resp, base$(&heap));

    if (result != 0)
    {
        BrMsg resp_msg;
        resp_msg.prot = binding.proto;
        resp_msg.type = BR_MSG_ERROR;
        resp_msg.args[0] = result;
        ipc_component_respond(self, msg, &resp_msg);
    }
    else if (resp != nullptr)
    {
        IpcPack pack;
        ipc_pack_init(&pack);

        binding.resp_pack(&pack, resp);

        BrMsg resp_msg;

        resp_msg.prot = binding.proto;
        resp_msg.type = binding.resp_id;
        resp_msg.flags = BR_MSG_HND(0);
        resp_msg.args[0] = pack.handle;

        ipc_component_respond(self, msg, &resp_msg);

        ipc_pack_deinit(&pack);
    }

    heap_alloc_deinit(&heap);
}
