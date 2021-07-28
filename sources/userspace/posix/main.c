#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>

int br_entry()
{
    log("Hello from the posix server!");

    BrIpcArgs ipc = {
        .flags = BR_IPC_RECV | BR_IPC_BLOCK,
        .timeout = -1,
    };

    br_ipc(&ipc);

    log("Got IPC message with proto:{} type:{} data:{}",
        ipc.message.header.protocol,
        ipc.message.header.type,
        ipc.message.data[0]);

    return 0;
}
