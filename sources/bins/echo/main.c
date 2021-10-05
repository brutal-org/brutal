#include <bal/ev.h>
#include <bal/syscalls.h>
#include <brutal/log.h>

void br_event(BrMsg const *msg)
{
    BrMsg resp = *msg;
    br_ev_resp(msg, &resp);
}

int br_entry()
{
    return br_ev_run();
}
