#pragma once

#include <bruwutal/text.h>
#include <ipc/ipc.h>

Str ipc_interface_str(IpcProto proto);

Str ipc_method_str(IpcProto proto, int method);
