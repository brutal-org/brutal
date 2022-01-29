#include <brutal/alloc.h>
#include <ipc/ipc.h>
#include <protos/bbus.h>
#include <protos/input.h>

/* --- Input Sink Protocol -------------------------------------------------- */

static InputError input_sink_handle_handler(void *, UiEvent const *req, bool *resp, Alloc *)
{
    log$("EVENT {}!", req->type);
    *resp = true;
    return IPC_SUCCESS;
}

static InputSinkVTable _input_sink_vtable = {
    input_sink_handle_handler,
};

/* --- Entry Point ---------------------------------------------------------- */

int ipc_component_main(IpcComponent *self)
{
    IpcCap bbus_server = ipc_component_require(self, IPC_BBUS_SERVER_PROTO);

    IpcCap input_sink_cap = input_sink_provide(self, &_input_sink_vtable, nullptr);

    bbus_server_expose(self, bbus_server, &input_sink_cap, alloc_global());

    log$("HELLO WORLD!");

    return ipc_component_run(self);
}
