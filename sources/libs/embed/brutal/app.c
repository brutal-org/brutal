#include <embed/app.h>
#include <embed/win.h>
#include <ipc/ipc.h>

void embed_app_init(UiApp *)
{
}

void embed_app_deinit(UiApp *)
{
}

void embed_app_pump(UiApp *)
{
}

void embed_app_wait(UiApp *)
{
    ipc_component_run(ipc_self());
}
