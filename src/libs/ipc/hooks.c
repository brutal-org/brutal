#include <bal/hw.h>
#include <ipc/hooks.h>
#include <ipc/ipc.h>
#include <brutal-alloc>
#include <brutal-debug>

int ipc_hook_call(
    IpcComponent *component,
    IpcCap to,
    IdlBinding binding,
    void const *req,
    void *resp,
    Alloc *alloc)
{
    BrMsg req_msg = {};
    BrMsg resp_msg = {};

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

    ipc_component_request(component, to, &req_msg, resp != nullptr ? &resp_msg : nullptr);

    // Error handeling

    if (resp_msg.type == BR_MSG_ERROR)
    {
        ipc_pack_deinit(&pack);
        return resp_msg.args[0];
    }

    // Unpacking

    if (resp != nullptr)
    {
        assert_equal(resp_msg.type, (BrArg)binding.resp_id);

        BalMem resp_mem;
        bal_mem_init_mobj(&resp_mem, resp_msg.args[0]);
        bal_mem_map(&resp_mem);

        IpcUnpack resp_unpack;
        ipc_unpack_init(&resp_unpack, resp_mem.buf, resp_mem.len, alloc);

        for (int i = 0; i < BR_MSG_ARG_COUNT - 1; i++)
        {
            resp_unpack.handles[i] = resp_msg.args[i + 1];
        }

        binding.resp_unpack(&resp_unpack, resp);
        bal_mem_deinit(&resp_mem);
    }

    ipc_pack_deinit(&pack);

    return 0;
}

void ipc_hook_handle(
    IpcComponent *component,
    IpcObject *object,
    IdlHandlerFn handler,

    BrMsg *msg,
    IdlBinding binding,

    void *req,
    void *resp)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    if (req != nullptr)
    {
        BalMem msg_mem;
        bal_mem_init_mobj(&msg_mem, msg->args[0]);
        bal_mem_map(&msg_mem);

        IpcUnpack msg_unpack;
        ipc_unpack_init(&msg_unpack, msg_mem.buf, msg_mem.len, base$(&heap));

        for (int i = 0; i < BR_MSG_ARG_COUNT - 1; i++)
        {
            msg_unpack.handles[i] = msg->args[i + 1];
        }

        binding.req_unpack(&msg_unpack, req);

        bal_mem_deinit(&msg_mem);
    }

    int result = handler(object, req, resp, base$(&heap));

    if (result != IPC_SUCCESS)
    {
        BrMsg resp_msg;
        resp_msg.prot = binding.proto;
        resp_msg.type = BR_MSG_ERROR;
        resp_msg.args[0] = result;
        ipc_component_respond(component, msg, &resp_msg);
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

        for (int i = 0; i < pack.handles_count; i++)
        {
            resp_msg.args[i + 1] = pack.handles[i];
            resp_msg.flags |= BR_MSG_HND(i + 1);
        }

        ipc_component_respond(component, msg, &resp_msg);

        ipc_pack_deinit(&pack);
    }

    heap_alloc_deinit(&heap);
}
