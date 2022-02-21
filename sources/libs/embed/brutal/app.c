#include <ipc/ipc.h>
#include <embed/app.h>
#include <embed/win.h>

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
    ipc_component_run(ipc_component_self());
}
