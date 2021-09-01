#include <brutal/log.h>
#include <syscalls/ev.h>
#include <syscalls/syscalls.h>

void br_event(BrMsg const *msg)
{
    BrMsg resp = *msg;
    br_ev_resp(msg, &resp);
}

int br_entry()
{
    return br_ev_run();
}
