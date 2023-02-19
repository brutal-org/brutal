#include <ipc/ipc.h>

int ipc_component_main(IpcComponent *self)
{
    return ipc_component_run(self);
}
